/* Fort Firewall Driver Configuration */

#include "fortconf.h"
#include "util.h"


#ifndef FORT_DRIVER
#define fort_memcmp	memcmp
#else
static int
fort_memcmp (const char *p1, const char *p2, size_t len)
{
  const size_t n = RtlCompareMemory(p1, p2, len);
  return (n == len) ? 0 : (p1[n] - p2[n]);
}
#endif

static BOOL
fort_conf_ip_inrange (UINT32 ip, UINT32 count, const UINT32 *iprange)
{
  int low, high;

  if (count == 0)
    return FALSE;

  low = 0, high = count - 1;

  do {
    const int mid = (low + high) / 2;
    const UINT32 mid_ip = iprange[mid];

    if (ip < mid_ip)
      high = mid - 1;
    else if (ip > mid_ip)
      low = mid + 1;
    else
      return TRUE;
  } while (low <= high);

  return high >= 0 && ip >= iprange[high]
    && ip <= iprange[count + high];
}

static const PFORT_CONF_ADDR_GROUP
fort_conf_addr_group_ref (const PFORT_CONF conf, int addr_group_index)
{
  const UINT32 *addr_group_offsets = (const UINT32 *)
    (conf->data + conf->addr_groups_off);
  const char *addr_group_data = (const char *) addr_group_offsets;

  return (PFORT_CONF_ADDR_GROUP)
    (addr_group_data + addr_group_offsets[addr_group_index]);
}

#define fort_conf_addr_group_include_ref(addr_group) \
  (addr_group)->ip

#define fort_conf_addr_group_exclude_ref(addr_group) \
  &(addr_group)->ip[(addr_group)->include_n * 2]

static BOOL
fort_conf_ip_included (const PFORT_CONF conf, UINT32 remote_ip,
                       int addr_group_index)
{
  const PFORT_CONF_ADDR_GROUP addr_group = fort_conf_addr_group_ref(
    conf, addr_group_index);

  const BOOL include_all = addr_group->include_all;
  const BOOL exclude_all = addr_group->exclude_all;

  const BOOL ip_included = include_all ? TRUE
    : fort_conf_ip_inrange(remote_ip, addr_group->include_n,
                           fort_conf_addr_group_include_ref(addr_group));

  const BOOL ip_excluded = exclude_all ? TRUE
    : fort_conf_ip_inrange(remote_ip, addr_group->exclude_n,
                           fort_conf_addr_group_exclude_ref(addr_group));

  return include_all ? !ip_excluded
    : (exclude_all ? ip_included
    : (ip_included && !ip_excluded));
}

#define fort_conf_ip_is_inet(conf, remote_ip) \
  fort_conf_ip_included((conf), (remote_ip), 0)

#define fort_conf_ip_inet_included(conf, remote_ip) \
  fort_conf_ip_included((conf), (remote_ip), 1)

static int
fort_conf_app_cmp (UINT32 path_len, const char *path,
                   const char *apps, const UINT32 *app_offp)
{
  const UINT32 app_off = *app_offp;
  const UINT32 app_len = *(app_offp + 1) - app_off;
  const char *app = apps + app_off;

  if (path_len > app_len)
    path_len = app_len;

  return fort_memcmp(path, app, path_len);
}

static int
fort_conf_app_index (const PFORT_CONF conf,
                     UINT32 path_len, const char *path)
{
  const char *data;
  const UINT32 *app_offsets;
  const char *apps;
  const UINT32 count = conf->apps_n;
  int low, high;

  if (count == 0)
    return -1;

  data = conf->data;
  app_offsets = (const UINT32 *) (data + conf->apps_off);

  apps = (const char *) (app_offsets + count + 1);
  low = 0, high = count - 1;

  do {
    const int mid = (low + high) / 2;
    const int res = fort_conf_app_cmp(path_len, path,
                                      apps, app_offsets + mid);

    if (res < 0)
      high = mid - 1;
    else if (res > 0)
      low = mid + 1;
    else
      return mid;
  } while (low <= high);

  return -1;
}

static UCHAR
fort_conf_app_group_index (const PFORT_CONF conf, int app_index)
{
  const char *data = conf->data;
  const UCHAR *app_groups = (const UCHAR *) (data + conf->app_groups_off);

  const BOOL app_found = (app_index != -1);

  return app_found ? app_groups[app_index] : 0;
}

static BOOL
fort_conf_app_blocked (const PFORT_CONF conf, int app_index)
{
  const char *data = conf->data;

  const UINT32 *app_perms = (const UINT32 *) (data + conf->app_perms_off);

  const BOOL block_all = conf->flags.app_block_all;
  const BOOL allow_all = conf->flags.app_allow_all;

  const BOOL app_found = (app_index != -1);

  const BOOL app_blocked = block_all ? TRUE : (app_found
    && (app_perms[app_index] & conf->app_perms_block_mask));
  const BOOL app_allowed = allow_all ? TRUE : (app_found
    && (app_perms[app_index] & conf->app_perms_allow_mask));

  return block_all ? !app_allowed
    : (allow_all ? app_blocked
    : (app_blocked && !app_allowed));
}

static UINT16
fort_conf_app_period_bits (const PFORT_CONF conf, int hour1, int hour2,
                           int *periods_n)
{
  const char *data;
  const CHAR *app_periods;
  UINT16 period_bits;
  UINT8 count = conf->app_periods_n;
  int n, i;

  if (count == 0)
    return 0;

  data = conf->data;
  app_periods = (const CHAR *) (data + conf->app_periods_off);
  period_bits = (UINT16) conf->flags.group_bits;
  n = 0;

  for (i = 0; i < FORT_CONF_GROUP_MAX; ++i) {
    const UINT16 bit = (1 << i);
    const int periodFrom = *app_periods++;
    const int periodTo = *app_periods++;

    if ((period_bits & bit) != 0
        && (periodFrom != 0 || periodTo != 0)) {
      if (!(is_hour_between(hour1, periodFrom, periodTo)
          && (hour1 == hour2 || is_hour_between(hour2, periodFrom, periodTo)))) {
        period_bits ^= bit;
      }

      ++n;

      if (--count == 0)
        break;
    }
  }

  if (periods_n != NULL) {
    *periods_n = n;
  }

  return period_bits;
}

static void
fort_conf_app_perms_mask_init (PFORT_CONF conf, UINT32 group_bits)
{
  UINT32 perms_mask =
       (group_bits & 0x0001)        | ((group_bits & 0x0002) << 1)
    | ((group_bits & 0x0004) << 2)  | ((group_bits & 0x0008) << 3)
    | ((group_bits & 0x0010) << 4)  | ((group_bits & 0x0020) << 5)
    | ((group_bits & 0x0040) << 6)  | ((group_bits & 0x0080) << 7)
    | ((group_bits & 0x0100) << 8)  | ((group_bits & 0x0200) << 9)
    | ((group_bits & 0x0400) << 10) | ((group_bits & 0x0800) << 11)
    | ((group_bits & 0x1000) << 12) | ((group_bits & 0x2000) << 13)
    | ((group_bits & 0x4000) << 14) | ((group_bits & 0x8000) << 15);

  perms_mask |= perms_mask << 1;

  conf->app_perms_block_mask = (perms_mask & 0xAAAAAAAA);
  conf->app_perms_allow_mask = (perms_mask & 0x55555555);
}
