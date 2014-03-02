/*
    This file is part of Pretty Curved Privacy (pcp1).

    Copyright (C) 2013 T.Linden.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    You can contact me by mail: <tlinden AT cpan DOT org>.
*/


#include "keyprint.h"


int pcptext_infile(char *infile) {
  FILE *in;
  int insize;

  if((in = fopen(infile, "rb")) == NULL) {
    fatal("Could not open input file %s\n", infile);
    goto errtinf1;
  }
  
  fseek(in, 0, SEEK_END);
  insize = ftell(in);
  fseek(in, 0, SEEK_SET);

  if(insize == 40) {
    fprintf(stdout, "%s seems to be an empty vault file\n", infile);
    goto tdone;
  }

  /*  maybe a vault? */
  vault_t *v = pcpvault_init(infile);
  if(v != NULL) {
    fprintf(stdout, "%s is a vault file\n", infile);
    pcptext_vault(v);
    goto tdone;
  }

  /*  try z85ing it */
  char *z85 = pcp_readz85file(in);
  if(z85 == NULL) {
      fprintf(stdout, "Can't handle %s - unknown file type.\n", infile); 
      goto errtinf1;
  }

  size_t clen;
  byte *bin = pcp_z85_decode((char *)z85, &clen);
  free(z85);

  if(bin == NULL) {
    fprintf(stdout, "%s isn't properly Z85 encoded - unknown file type.\n", infile);
    goto errtinf1;
  }

  /* FIXME: try to import pk or sk */

  /*  still there? */
  fprintf(stdout, "%s looks Z85 encoded but otherwise unknown and is possibly encrypted.\n", infile);

 tdone:
  fatals_reset();
  return 0;

 errtinf1:
  fatals_reset();
  return 1;
}


void pcptext_key(char *keyid) {
  pcp_key_t *s = pcphash_keyexists(keyid);
  if(s != NULL) {
    if(debug)
      pcp_dumpkey(s);
    pcpkey_print(s, stdout);
  }
  else {
    pcp_pubkey_t *p = pcphash_pubkeyexists(keyid);
    if(p != NULL) {
      if(debug)
	pcp_dumppubkey(p);
      pcppubkey_print(p, stdout);
    }
    else {
      fatal("No key with id 0x%s found!\n", keyid);
    }
  }
}

void pcptext_vault(vault_t *vault) {
  printf("    Key vault: %s\n", vault->filename);
  printf("Vault version: %08X\n", vault->version);
  printf("     Checksum: ");

  int i;
  for ( i = 0;i <15 ;++i) printf("%02X:",(unsigned int) vault->checksum[i]);
  printf("%02X", vault->checksum[15]);
  printf("\n               ");
  for ( i = 16;i <31 ;++i) printf("%02X:",(unsigned int) vault->checksum[i]);
  printf("%02X", vault->checksum[31]);
  printf("\n");

  printf("  Secret keys: %d\n", HASH_COUNT(pcpkey_hash));
  printf("  Public keys: %d\n",  HASH_COUNT(pcppubkey_hash));
}

void pcpkey_printlineinfo(pcp_key_t *key) {
  struct tm *c;
  time_t t = (time_t)key->ctime;
  c = localtime(&t);
  printf("0x%s   %s   %04d-%02d-%02dT%02d:%02d:%02d  %s <%s>\n",
	 key->id,
	 (key->type ==  PCP_KEY_TYPE_MAINSECRET) ? "primary secret" : "secret        ",  
	 c->tm_year+1900, c->tm_mon+1, c->tm_mday,
	 c->tm_hour, c->tm_min, c->tm_sec,
	 key->owner, key->mail);

  if(PCPVERBOSE) {
    printf("    ");
    byte *hash = pcpkey_getchecksum(key);
    int i, y;
    for(i=0; i<32; i+=4) {
      for(y=0; y<4; y++) {
	printf("%02x", hash[i+y]);
      }
      printf(" ");
    }
    free(hash);
    printf("\n    encrypted: %s, serial: %08x, version: %d\n",
	   (key->secret[0] == '\0') ? "yes" : " no",
	   key->serial, (int)key->version);
    printf("\n");
  }
}

void pcppubkey_printlineinfo(pcp_pubkey_t *key) {
  struct tm *c;
  time_t t = (time_t)key->ctime;
  c = localtime(&t);
  printf("0x%s   %s   %04d-%02d-%02dT%02d:%02d:%02d  %s <%s>\n",
	 key->id,
	 (key->valid == 1) ? "valid public  " : "public        ",  
	 c->tm_year+1900, c->tm_mon+1, c->tm_mday,
	 c->tm_hour, c->tm_min, c->tm_sec,
	 key->owner, key->mail);

  if(PCPVERBOSE) {
    printf("    ");
    byte *hash = pcppubkey_getchecksum(key);
    int i, y;
    for(i=0; i<32; i+=4) {
      for(y=0; y<4; y++) {
	printf("%02x", hash[i+y]);
      }
      printf(" ");
    }
    free(hash);
    printf("\n    signed: %s, serial: %08x, version: %d, ",
	   (key->valid == 1) ? "yes" : " no",
	   key->serial, (int)key->version);
    pcp_keysig_t *sig = pcphash_keysigexists(key->id);
    if(sig != NULL) {
      printf("signature fingerprint:\n    ");
      byte *checksum = sig->checksum;
      for(i=0; i<32; i+=4) {
	for(y=0; y<4; y++) {
	  printf("%02x", checksum[i+y]);
	}
	printf(" ");
      }
      printf("\n");
    }
    else {
      printf("fail: no signature stored.\n");
    }

    printf("\n");
  }
}

void pcppubkey_print(pcp_pubkey_t *key, FILE* out) {
  size_t zlen;
  struct tm *c;
  time_t t = (time_t)key->ctime;
  c = localtime(&t);

  fprintf(out, " Cipher: %s\n", PCP_KEY_PRIMITIVE);

  fprintf(out, " Owner: %s\n", key->owner);
  fprintf(out, " Mail: %s\n", key->mail);

  fprintf(out, " Key-ID: 0x%s\n", key->id);
  fprintf(out, " Public-Key: %s\n", pcp_z85_encode(key->pub, 32, &zlen));

  /* 2004-06-14T23:34:30. */
  fprintf(out, " Creation Time: %04d-%02d-%02dT%02d:%02d:%02d\n",
	  c->tm_year+1900, c->tm_mon+1, c->tm_mday,
	  c->tm_hour, c->tm_min, c->tm_sec);

  byte *hash = pcppubkey_getchecksum(key);
  fprintf(out, " Checksum: ");

  size_t i;
  for ( i = 0;i <15 ;++i) fprintf(out, "%02X:",(unsigned int) hash[i]);
  fprintf(out, "%02X", hash[15]);
  fprintf(out, "\n ");
  for ( i = 16;i <31 ;++i) fprintf(out, "%02X:",(unsigned int) hash[i]);
  fprintf(out, "%02X", hash[31]);
  fprintf(out, "\n");
  fprintf(out, " Serial Number: 0x%08X\n", key->serial);
  fprintf(out, " Key Version: 0x%08X\n", key->version);
  
  char *r = pcppubkey_get_art(key);
  fprintf(out, " Random Art ID: ");
  size_t rlen = strlen(r);
  for (i=0; i<rlen; ++i) {
    if(r[i] == '\n') {
      fprintf(out, "\n ");
    }
    else {
      fprintf(out, "%c", r[i]);
    }
  }
  fprintf(out, "\n");
  
  free(hash);
  free(r);
}

void pcpkey_print(pcp_key_t *key, FILE* out) {
  struct tm *c;
  time_t t = (time_t)key->ctime;
  c = localtime(&t);

  fprintf(out, " Cipher: %s\n", PCP_KEY_PRIMITIVE);

  fprintf(out, " Key-ID: 0x%s\n", key->id);

  /* 2004-06-14T23:34:30. */
  fprintf(out, " Creation Time: %04d-%02d-%02dT%02d:%02d:%02d\n",
	  c->tm_year+1900, c->tm_mon+1, c->tm_mday,
	  c->tm_hour, c->tm_min, c->tm_sec);

  fprintf(out, " Serial Number: 0x%08X\n", key->serial);
  fprintf(out, " Key Version: 0x%08X\n", key->version);
  
}

void pcpkey_printshortinfo(pcp_key_t *key) {
  size_t i;
  printf("        Key-ID: 0x%s\n", key->id);
  printf("         Owner: %s\n", key->owner);
  char *r = pcpkey_get_art(key);
  printf(" Random Art ID: ");
  size_t rlen = strlen(r);
  for (i=0; i<rlen; ++i) {
    if(r[i] == '\n') {
      printf("\n                ");
    }
    else {
      printf("%c", r[i]);
    }
  }
  printf("\n");
  free(r);
}

void pcppubkey_printshortinfo(pcp_pubkey_t *key) {
  size_t i;
  printf("        Key-ID: 0x%s\n", key->id);
  printf("         Owner: %s\n", key->owner);
  char *r = pcppubkey_get_art(key);
  printf(" Random Art ID: ");
  size_t rlen = strlen(r);
  for (i=0; i<rlen; ++i) {
    if(r[i] == '\n') {
      printf("\n                ");
    }
    else {
      printf("%c", r[i]);
    }
  }
  printf("\n");
  free(r);
}

void pcpexport_yaml(char *outfile) {
  FILE *out;

  if(outfile == NULL) {
    out = stdout;
  }
  else {
    if((out = fopen(outfile, "wb+")) == NULL) {
      fatal("Could not create output file %s", outfile);
      out = NULL;
    }
  }

  if(out != NULL) {
    pcp_key_t *s;
    pcp_pubkey_t *p;

    struct tm *c;
    time_t t = time(0);
    c = localtime(&t);

    fprintf(out, "#\n# YAML export of vault %s.\n", vault->filename);
    fprintf(out, "# Generated on: %04d-%02d-%02dT%02d:%02d:%02d\n",
	    c->tm_year+1900, c->tm_mon+1, c->tm_mday,
	    c->tm_hour, c->tm_min, c->tm_sec);
    fprintf(out, "---\n");
    fprintf(out, "secret-keys:\n");

    pcphash_iterate(s) {
      fprintf(out, " -\n");
      fprintf(out, "  id:         %s\n", s->id);
      fprintf(out, "  owner:      %s\n", s->owner);
      fprintf(out, "  mail:       %s\n", s->mail);
      fprintf(out, "  ctime:      %ld\n", (long int)s->ctime);
      fprintf(out, "  version:    %08x\n", s->version);
      fprintf(out, "  serial:     %08x\n", s->serial);
      fprintf(out, "  type:       %s\n",
	      (s->type ==  PCP_KEY_TYPE_MAINSECRET) ? "primary" : " secret");
      fprintf(out, "  public:     "); pcpprint_bin(out, s->pub, 32); fprintf(out, "\n");
      if(s->secret[0] == 0) {
	fprintf(out, "  encrypted:  yes\n");
	fprintf(out, "  nonce:      "); pcpprint_bin(out, s->nonce, 24); fprintf(out, "\n");
	fprintf(out, "  secret:     "); pcpprint_bin(out, s->encrypted, 80); fprintf(out, "\n");
      }
      else {
	fprintf(out, "  encrypted:  no\n");
	fprintf(out, "  secret:     "); pcpprint_bin(out, s->secret, 32); fprintf(out, "\n");
	fprintf(out, "  edsecret:   "); pcpprint_bin(out, s->edsecret, 64); fprintf(out, "\n");
      }
      fprintf(out, "  edpub:      "); pcpprint_bin(out, s->edpub, 32); fprintf(out, "\n");
    }
    
    fprintf(out, "public-keys:\n");
    pcphash_iteratepub(p) {
      fprintf(out, " -\n");
      fprintf(out, "  id:      %s\n", p->id);
      fprintf(out, "  owner:   %s\n", p->owner);
      fprintf(out, "  mail:    %s\n", p->mail);
      fprintf(out, "  ctime:   %ld\n", (long int)p->ctime);
      fprintf(out, "  version: %08x\n", p->version);
      fprintf(out, "  serial:  %08x\n", p->serial);
      fprintf(out, "  type:    public\n");
      fprintf(out, "  public:  "); pcpprint_bin(out, p->pub, 32); fprintf(out, "\n");
      fprintf(out, "  edpub:   "); pcpprint_bin(out, p->edpub, 32); fprintf(out, "\n");
    }
  }
}

void pcpprint_bin(FILE *out, byte *data, size_t len) {
  size_t i;
  for ( i = 0;i < len;++i)
    fprintf(out, "%02x", (unsigned int) data[i]);
}
