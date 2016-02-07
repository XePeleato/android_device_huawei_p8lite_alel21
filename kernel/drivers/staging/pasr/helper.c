/*
 * Copyright (C) ST-Ericsson SA 2011
 * Author: Maxime Coquelin <maxime.coquelin@stericsson.com> for ST-Ericsson.
 * License terms:  GNU General Public License (GPL), version 2
 */

#include <linux/pasr.h>


struct pasr_die *pasr_addr2die(struct pasr_map *map, phys_addr_t addr)
{
	unsigned int left, right, mid;

	if (!map)
		return NULL;

	left = 0;
	right = map->nr_dies;

	//addr &= ~(PASR_SECTION_SZ - 1);
	//addr = (addr / PASR_SECTION_SZ) * PASR_SECTION_SZ;

	while (left != right) {
		struct pasr_die *d;
		phys_addr_t start;

		mid = (left + right) >> 1;
		d = &map->die[mid];
		start = (addr / (d->section_size * d->nr_sections)) * (d->section_size * d->nr_sections);

		if (start == d->start)
			return d;
		else if (start > d->start)
			left = mid;
		else
			right = mid;
	}

	pr_err("%s: No die found for address %#x",
			__func__, addr);
	return NULL;
}

struct pasr_section *pasr_addr2section(struct pasr_map *map
				, phys_addr_t addr)
{
	unsigned int left, right, mid;
	struct pasr_die *die;

	/* Find the die the address it is located in */
	die = pasr_addr2die(map, addr);
	if (!die)
		goto err;

	left = 0;
	right = die->nr_sections;

	//addr &= ~(PASR_SECTION_SZ - 1);
	addr = (addr / die->section_size) * die->section_size;

	 while (left != right) {
		struct pasr_section *s;

		mid = (left + right) >> 1;
		s = &die->section[mid];

		if (addr == s->start)
			return s;
		else if (addr > s->start)
			left = mid;
		else
			right = mid;
	}

err:
	/* Provided address isn't in any declared section */
	pr_err("%s: No section found for address %#x",
			__func__, addr);

	return NULL;
}

phys_addr_t pasr_section2addr(struct pasr_section *s)
{
	return s->start;
}
