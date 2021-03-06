/* ISC license. */

#include <errno.h>
#include <skalibs/uint16.h>
#include <skalibs/bytestr.h>
#include <skalibs/error.h>
#include <s6-networking/ident.h>

static unsigned int skipspace (char const *s)
{
  register unsigned int n = 0 ;
  while ((s[n] == ' ') || (s[n] == '\t')) n++ ;
  return n ;
}

int s6net_ident_reply_parse (char const *s, uint16 rp, uint16 lp)
{
  unsigned int n = 0 ;
  n += skipspace(s+n) ; if (!s[n]) goto err ;
  {
    unsigned int i ;
    uint16 u ;
    i = uint16_scan(s+n, &u) ; if (!i) goto err ; n += i ;
    if (u != rp) goto err ;
    n += skipspace(s+n) ; if (!s[n]) goto err ;
    if (s[n++] != ',') goto err ;
    n += skipspace(s+n) ; if (!s[n]) goto err ;
    i = uint16_scan(s+n, &u) ; if (!i) goto err ; n += i ;
    if (u != lp) goto err ;
  }
  n += skipspace(s+n) ; if (!s[n]) goto err ;
  if (s[n++] != ':') goto err ;
  n += skipspace(s+n) ; if (!s[n]) goto err ;
  if (!str_diffn(s+n, "ERROR", 5)) goto ERROR ;
  if (!str_diffn(s+n, "USERID", 6)) goto USERID ;
 err:
  return (errno = EPROTO, -1) ;

 ERROR:
  n += 5 ;
  n += skipspace(s+n) ; if (!s[n]) goto err ;
  if (s[n++] != ':') goto err ;
  n += skipspace(s+n) ; if (!s[n]) goto err ;
  if (!str_diffn(s+n, "INVALID-PORT", 12)) return (errno = EINVAL, 0) ;
  if (!str_diffn(s+n, "NO-USER", 7)) return (errno = ESRCH, 0) ;
  if (!str_diffn(s+n, "HIDDEN-USER", 11)) return (errno = EPERM, 0) ;
  if (!str_diffn(s+n, "UNKNOWN-ERROR", 13)) return (errno = EIO, 0) ;
  if (s[n] == 'X') return (errno = EEXIST, 0) ;
  goto err ;

 USERID:
  n += 6 ;
  n += skipspace(s+n) ; if (!s[n]) goto err ;
  if (s[n++] != ':') goto err ;
  n += str_chr(s+n, ':') ; if (!s[n]) goto err ;
  n++ ; if ((s[n] == ' ') || (s[n] == '\t')) n++ ;
  return n ;
}
