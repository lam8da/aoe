#include <alloca.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "xmap.h"
#include "rsrc.h"

#define RSRC_NLEAF 16

#ifdef RSRCDBG
#define dbgs(s) puts(s)
#define fdbgs(s,f) fputs(s,f)
#define dbgf(f,...) printf(f, ##__VA_ARGS__)
#else
#define dbgs(s) ((void)0)
#define fdbgs(s,f) ((void)0)
#define dbgf(f,...) ((void)0)
#endif

struct strheap strtbl;

#define parent(x) (((x)-1)/2)
#define right(x) (2*((x)+1))
#define left(x) (right(x)-1)

static inline void swap(struct strheap *h, unsigned a, unsigned b)
{
	struct rstrptr tmp;
	tmp = h->a[a];
	h->a[a] = h->a[b];
	h->a[b] = tmp;
}

static inline int rptrcmp(struct rstrptr *a, struct rstrptr *b)
{
	return a->id - b->id;
}

static inline void siftup(struct strheap *h, unsigned i)
{
	register unsigned j;
	register int cmp;
	while (i) {
		j = parent(i);
		if ((cmp = rptrcmp(&h->a[j], &h->a[i])) > 0)
			swap(h, j, i);
		else
			break;
		i = j;
	}
}

static inline void put(struct strheap *h, struct rstrptr *i)
{
	assert(h->n < HEAPSZ);
	// naively search for existing id
	for (register unsigned j = 0; j < h->n; ++j)
		if (h->a[j].id == i->id) {
			h->a[j] = *i;
			return;
		}
	h->a[h->n] = *i;
	siftup(h, h->n++);
}

void libdump(void)
{
	unsigned i;
	const struct strheap *h = &strtbl;
	dbgf("heap len=%d\n", h->n);
	for (i = 0; i < h->n; ++i)
		dbgf(" %d", h->a[i].id);
	putchar('\n');
}

static int rsrc_strtbl(struct xfile *x, unsigned level, off_t diff, size_t off)
{
	char *map = x->data;
	size_t mapsz = x->size;
	struct rsrcitem *i = (struct rsrcitem*)(map + off);
	if (off + sizeof(struct rsrcitem) > mapsz) {
		fprintf(stderr, "bad leaf at %zX: file too small\n", off);
		return 1;
	}
	if (diff < 0 && diff + (ssize_t)i->d_rva < 0) {
		fprintf(stderr, "bad leaf rva -%zX\n", (size_t)-diff);
		return 1;
	}
	dbgf("%8zX ", off);
	for (unsigned l = 0; l < level; ++l)
		fdbgs("  ", stdout);
	size_t pp, p = i->d_rva + diff;
	pp = p;
	dbgf("strtbl at %zX: %zX bytes\n", p, (size_t)i->d_size);
	uint16_t j, k, n, w, *hw;
	char *str, *buf;
	for (k = 0, buf = alloca(65536); p < pp + i->d_size; ++k) {
		hw = (uint16_t*)(map + p);
		p += sizeof(uint16_t);
		w = *hw;
		dbgf("%8zX ", p);
		for (unsigned l = 0; l < level; ++l)
			fdbgs("  ", stdout);
		if (w) {
			if (p + 2 * w > mapsz) {
				fprintf(stderr, "bad leaf at %zX: file too small\n", off);
				return 1;
			}
			dbgf("#%2u ", k);
			for (str = map + p, j = 0, n = w; j < n; str += 2)
				buf[j++] = *str;
			buf[j++] = '\0';
			dbgs(buf);
			struct rstrptr ptr;
			ptr.id = strtbl.str_id + k;
			ptr.str.length = w;
			ptr.str.str = (map + p);
			dbgf("rsrc heap: put (%u,%s) @%zX\n", ptr.id, buf, p);
			dbgf("rsrc heap: str: %u\n", ptr.str.length);
			dbgf("rsrc heap: %c\n", ptr.str.str);
			put(&strtbl, &ptr);
		} else
			dbgf("#%2u\n", k);
		p += w * 2;
	}
	return 0;
}

static int rsrc_leaf(struct xfile *x, unsigned level, size_t soff, off_t diff, size_t *off)
{
	char *map = x->data;
	size_t mapsz = x->size;
	if (*off + sizeof(struct rsrcditem) > mapsz) {
		fprintf(stderr, "bad leaf at %zX: file too small\n", *off);
		return 1;
	}
	struct rsrcditem *ri = (struct rsrcditem*)(map + *off);
	size_t loff = soff + ri->r_rva;
	if (rsrc_strtbl(x, level, diff, loff)) {
		fprintf(stderr, "corrupt strtbl at %zX\n", loff);
		return 1;
	}
	*off += sizeof(struct rsrcditem);
	return 0;
}

static int rsrc_walk(struct xfile *x, unsigned level, size_t soff, off_t diff, size_t *off, unsigned tn_id, size_t *poff, size_t *sn)
{
	char *map = x->data;
	size_t size = x->size;
	(void)tn_id;
	dbgf("soff=%zX,off=%zX\n", soff, *off);
	if (*off + sizeof(struct rsrcdir) > size) {
		fprintf(stderr, "bad rsrc node at level %u: file too small\n", level);
		return 1;
	}
	if (level == 3) {
		fputs("bad rsrc: too many levels\n", stderr);
		return 1;
	}
	struct rsrcdir *d = (struct rsrcdir*)(map + *off);
	dbgf("%8zX ", *off);
	for (unsigned l = 0; l < level; ++l)
		fdbgs("  ", stdout);
	dbgf(
		"ResDir Named:%02X ID:%02X TimeDate:%8X Vers:%u.%02u Char:%u\n",
		d->r_nnment, d->r_nident, d->r_timdat, d->r_major, d->r_minor, d->r_flags
	);
	unsigned n = d->r_nnment + d->r_nident;
	*off += sizeof(struct rsrcdir);
	if (*off + n * sizeof(struct rsrcdir) > size) {
		fprintf(stderr, "bad rsrc node at level %u: file too small\n", level);
		return 1;
	}
	int dir = 0;
	size_t poffp = *poff;
	for (unsigned i = 0; i < n; ++i) {
		struct rsrcditem *ri = (struct rsrcditem*)(map + *off);
		dbgf("rva item = %zX\n", *off);
		if (dir) {
			if (rsrc_walk(x, level + 1, soff, diff, off, i < d->r_nnment ? TN_NAME : TN_ID, &poffp, sn))
				return 1;
			poffp += sizeof(struct rsrcditem);
		} else {
			dbgf("%8zX ", *off);
			for (unsigned l = 0; l < level; ++l)
				fdbgs("  ", stdout);
			dbgf("id=%u,rva=%X,type=%s,poff=%zX,poffp=%zX,%u\n", ri->r_id, ri->r_rva, tn_id == TN_ID ? "id" : "name", *poff, poffp, level);
			// black magic
			if (level == 2) {
				if (*sn != 0) {
					if (poffp + sizeof(struct rsrcditem) >= size) {
						fprintf(stderr, "bad offset: %zX\n", poffp);
						return 1;
					}
					struct rsrcditem *m = (struct rsrcditem*)(map + poffp);
					strtbl.str_id = (m->r_id - 1) * 16;
					dbgf("resid=%u\n", strtbl.str_id);
				}
				++*sn;
			} else if (level == 1) {
				strtbl.str_id = (ri->r_id - 1) * 16;
				dbgf("resid=%u\n", strtbl.str_id);
			}
			poffp = *off + sizeof(struct rsrcditem);
			if ((ri->r_rva >> 31) & 1) {
				size_t roff = soff + (ri->r_rva & ~(1 << 31));
				if (rsrc_walk(x, level + 1, soff, diff, &roff, i < d->r_nnment ? TN_NAME : TN_ID, &poffp, sn))
					return 1;
				*off = roff;
				dbgf("roff=%zX\n", roff);
				dir = 1;
			} else {
				dbgf("%8zX ", *off);
				for (unsigned l = 0; l < level; ++l)
					fdbgs("  ", stdout);
				dbgf("#%u ID: %8X Offset: %8X\n", i, ri->r_id, ri->r_rva);
				if (rsrc_leaf(x, level + 1, soff, diff, off))
					return 1;
			}
		}
	}
	return 0;
}

static int rsrc_stat(struct xfile *x)
{
	struct sechdr *sec = x->sec;
	if (!sec) {
		fputs("internal error\n", stderr);
		return 1;
	}
	unsigned i;
	for (i = 0; i < x->peopt->o_nrvasz; ++i, ++sec) {
		char name[9];
		strncpy(name, sec->s_name, 9);
		name[8] = '\0';
		if (!strcmp(name, ".rsrc"))
			goto found;
	}
	fputs("no .rsrc\n", stderr);
	return 1;
found:
	dbgf("goto %u@%zX\n", i, (size_t)sec->s_scnptr);
	size_t off = sec->s_scnptr, poff = off;
	size_t n = 0;
	off_t diff = (ssize_t)sec->s_scnptr - sec->s_vaddr;
	int ret = rsrc_walk(x, 0, sec->s_scnptr, diff, &off, 0, &poff, &n);
	dbgf("node count: %zu\n", n);
	return ret;
}

int loadlib(const char *name, char **data, size_t *size)
{
	int ret = -1, fd = -1;
	char *map = MAP_FAILED;
	size_t mapsz = 0;
	struct xfile x;
	if (xmap(name, PROT_READ, &fd, &map, &mapsz))
		return 1;
	if ((ret = xstat(&x, map, mapsz)) != 0)
		goto fail;
	if (x.type != XT_PEOPT) {
		fprintf(stderr, "%s: type %u, expected %u\n", name, x.type, XT_PEOPT);
		goto fail;
	}
	ret = rsrc_stat(&x);
fail:
	if (ret)
		xunmap(fd, map, mapsz);
	else {
		*data = map;
		*size = mapsz;
	}
	return ret ? -1 : fd;
}
