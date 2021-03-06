/*
 * eoptunnel.c
 *
 * Copyright (C) 2005 - 2018 Sebastian Gottschall <gottschall@dd-wrt.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id:
 */
#ifdef HAVE_EOP_TUNNEL

#define VISUALSOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include <broadcom.h>

void show_ip(webs_t wp, char *prefix, char *var, int nm, char *type);
void show_caption(webs_t wp, const char *class, const char *cap, const char *ext);
void show_caption_simple(webs_t wp, const char *cap);

void ej_show_eop_tunnels(webs_t wp, int argc, char_t ** argv)
{

	int tun;
	char temp[128];
	char temp2[128];
	int tunnels = nvram_default_geti("oet_tunnels", 0) + 1;

	for (tun = 1; tun < tunnels; tun++) {
		char oet[32];
		sprintf(oet, "oet%d", tun);
		websWrite(wp, "<fieldset>\n");
		websWrite(wp, "<legend>");
		show_caption_simple(wp, "eoip.tunnel");
		websWrite(wp, " %s</legend>\n", getNetworkLabel(wp, oet));
		websWrite(wp, "<div class=\"setting\">\n");
		{
			show_caption(wp, "label", "eoip.srv", NULL);
			snprintf(temp, sizeof(temp), "oet%d_en", tun);
			websWrite(wp,
				  "<input class=\"spaceradio\" type=\"radio\" value=\"1\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idoet%d', true)\" />", temp, (nvram_matchi(temp, 1) ? "checked=\"checked\"" : ""),
				  tun);
			show_caption(wp, NULL, "share.enable", "&nbsp;");
			websWrite(wp,
				  "<input class=\"spaceradio\" type=\"radio\" value=\"0\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idoet%d', false)\" />", temp, (nvram_matchi(temp, 0) ? "checked=\"checked\"" : ""),
				  tun);
			show_caption_simple(wp, "share.disable");
		}
		websWrite(wp, "</div>\n");
		websWrite(wp, "<div id=\"idoet%d\">\n", tun);
		{
			snprintf(temp, sizeof(temp), "oet%d_proto", tun);
			websWrite(wp, "<div class=\"setting\">\n");
			{
				show_caption(wp, "label", "eoip.proto", NULL);
				websWrite(wp, "<select name=\"oet%d_proto\" onclick=\"changeproto(this, %d, this.value, %s)\">\n", tun, tun, nvram_nget("oet%d_bridged", tun));

				websWrite(wp, "<script type=\"text/javascript\">\n//<![CDATA[\n");
				websWrite(wp, "document.write(\"<option value=\\\"0\\\" %s >\"  + eoip.etherip + \"</option>\");\n", nvram_match(temp, "0") ? "selected=\\\"selected\\\"" : "");
				websWrite(wp, "document.write(\"<option value=\\\"1\\\" %s >\"  + eoip.mtik + \"</option>\");\n", nvram_match(temp, "1") ? "selected=\\\"selected\\\"" : "");
#ifdef HAVE_WIREGUARD
				websWrite(wp, "document.write(\"<option value=\\\"2\\\" %s >\"  + eoip.wireguard + \"</option>\");\n", nvram_match(temp, "2") ? "selected=\\\"selected\\\"" : "");
#endif
				websWrite(wp, "//]]>\n</script>\n");
				websWrite(wp, "</select>\n");
			}
			websWrite(wp, "</div>\n");
			websWrite(wp, "<div id=\"idmtik%d\">\n", tun);
			{
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "eoip.tunnelID", NULL);
					snprintf(temp, sizeof(temp), "oet%d_id", tun);
					websWrite(wp, "<input size=\"4\" maxlength=\"3\" class=\"num\" name=\"%s\" onblur=\"valid_range(this,0,65535,eoip.tunnelID)\" value=\"%s\" />\n", temp, nvram_get(temp));
				}
				websWrite(wp, "</div>\n");
			}
			websWrite(wp, "</div>\n");
			websWrite(wp, "<div id=\"idwireguard%d\">\n", tun);
			{
#ifdef HAVE_WIREGUARD
				snprintf(temp, sizeof(temp), "oet%d_port", tun);
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "eoip.wireguard_localport", NULL);
					websWrite(wp, "<input size=\"5\" maxlength=\"5\" name=\"%s\" class=\"num\" value=\"%s\" />\n", temp, nvram_safe_get(temp));
				}
				websWrite(wp, "</div>\n");

				websWrite(wp, "<div class=\"center\">\n");
				{
					websWrite(wp, "<script type=\"text/javascript\">\n//<![CDATA[\n");
					websWrite(wp,
						  "document.write(\"<input class=\\\"button\\\" type=\\\"button\\\" name=\\\"gen_key_button\\\" value=\\\"\" + eoip.genkey + \"\\\" onclick=\\\"gen_wg_key(this.form,%d)\\\" />\");\n",
						  tun);
					websWrite(wp, "//]]>\n</script>\n");
				}
				websWrite(wp, "</div>\n");

				//public key show
				snprintf(temp, sizeof(temp), "oet%d_public", tun);
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "eoip.wireguard_localkey", NULL);
					websWrite(wp, "<input size=\"48\" maxlength=\"48\" name=\"%s\" value=\"%s\" disabled=\"disabled\"/>\n", temp, nvram_safe_get(temp));
				}
				websWrite(wp, "</div>\n");
				snprintf(temp, sizeof(temp), "oet%d_peers", tun);
				nvram_default_get(temp, "0");
				int peers = nvram_geti(temp);
				int peer;
				for (peer = 0; peer < peers; peer++) {
					{
						websWrite(wp, "<fieldset>\n");
						websWrite(wp, "<legend>Peer %d</legend>\n", peer + 1);
						snprintf(temp, sizeof(temp), "oet%d_endpoint%d", tun, peer);
						nvram_default_get(temp, "0");
						websWrite(wp, "<div class=\"setting\">\n");
						{
							show_caption(wp, "label", "eoip.wireguard_endpoint", NULL);
							websWrite(wp,
								  "<input class=\"spaceradio\" type=\"radio\" value=\"1\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idendpoint%d_peer%d', true)\" />", temp,
								  (nvram_matchi(temp, 1) ? "checked=\"checked\"" : ""), tun, peer);
							show_caption(wp, NULL, "share.enable", "&nbsp;");
							websWrite(wp,
								  "<input class=\"spaceradio\" type=\"radio\" value=\"0\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idendpoint%d_peer%d', false)\" />", temp,
								  (nvram_matchi(temp, 0) ? "checked=\"checked\"" : ""), tun, peer);
							show_caption_simple(wp, "share.disable");
						}
						websWrite(wp, "</div>\n");
						websWrite(wp, "<div id=\"idendpoint%d_peer%d\">\n", tun, peer);
						{

							snprintf(temp2, sizeof(temp2), "oet%d_peerport%d", tun, peer);
							snprintf(temp, sizeof(temp), "oet%d_rem%d", tun, peer);
							websWrite(wp, "<div class=\"setting\">\n");
							{
								show_caption(wp, "label", "eoip.wireguard_peer", NULL);
								websWrite(wp,
									  "<input size=\"20\" maxlength=\"20\" name=\"%s\" value=\"%s\" />:<input size=\"5\" maxlength=\"5\" name=\"%s\" class=\"num\" value=\"%s\" />\n\n",
									  temp, nvram_safe_get(temp), temp2, nvram_safe_get(temp2));
							}
							websWrite(wp, "</div>\n");
						}
						websWrite(wp, "</div>\n");
						snprintf(temp, sizeof(temp), "oet%d_aip%d", tun, peer);
						websWrite(wp, "<div class=\"setting\">\n");
						{
							show_caption(wp, "label", "eoip.wireguard_allowedips", NULL);
							websWrite(wp, "<input size=\"18\" maxlength=\"18\" name=\"%s\" value=\"%s\" />\n", temp, nvram_default_get(temp, "0.0.0.0/0"));
						}
						websWrite(wp, "</div>\n");
						snprintf(temp, sizeof(temp), "oet%d_ka%d", tun, peer);
						websWrite(wp, "<div class=\"setting\">\n");
						{
							show_caption(wp, "label", "eoip.wireguard_ka", NULL);
							websWrite(wp, "<input size=\"5\" maxlength=\"5\" name=\"%s\" class=\"num\" onblur=\"valid_range(this,0,65535,eoip.wireguard_ka)\" value=\"%s\" />\n", temp,
								  nvram_safe_get(temp));
						}
						websWrite(wp, "</div>\n");

						//public key peer input
						snprintf(temp, sizeof(temp), "oet%d_peerkey%d", tun, peer);
						websWrite(wp, "<div class=\"setting\">\n");
						{
							show_caption(wp, "label", "eoip.wireguard_peerkey", NULL);
							websWrite(wp, "<input size=\"48\" maxlength=\"48\" name=\"%s\" value=\"%s\" />\n", temp, nvram_safe_get(temp));
						}
						websWrite(wp, "</div>\n");
						snprintf(temp, sizeof(temp), "oet%d_usepsk%d", tun, peer);
						nvram_default_get(temp, "0");
						websWrite(wp, "<div class=\"setting\">\n");
						{
							show_caption(wp, "label", "eoip.wireguard_usepsk", NULL);
							websWrite(wp,
								  "<input class=\"spaceradio\" type=\"radio\" value=\"1\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idpsk%d_peer%d', true)\" />", temp,
								  (nvram_matchi(temp, 1) ? "checked=\"checked\"" : ""), tun, peer);

							show_caption(wp, NULL, "share.enable", "&nbsp;");

							websWrite(wp,
								  "<input class=\"spaceradio\" type=\"radio\" value=\"0\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idpsk%d_peer%d', false)\" />", temp,
								  (nvram_matchi(temp, 0) ? "checked=\"checked\"" : ""), tun, peer);
							show_caption_simple(wp, "share.disable");
						}
						websWrite(wp, "</div>\n");
						websWrite(wp, "<div id=\"idpsk%d_peer%d\">\n", tun, peer);
						{

							websWrite(wp, "<div class=\"center\">\n");
							{
								websWrite(wp, "<script type=\"text/javascript\">\n//<![CDATA[\n");
								websWrite(wp,
									  "document.write(\"<input class=\\\"button\\\" type=\\\"button\\\" name=\\\"gen_psk_button\\\" value=\\\"\" + eoip.wireguard_genpsk + \"\\\" onclick=\\\"gen_wg_psk(this.form,%d,%d)\\\" />\");\n",
									  tun, peer);
								websWrite(wp, "//]]>\n</script>\n");
							}
							websWrite(wp, "</div>\n");

							snprintf(temp, sizeof(temp), "oet%d_psk%d", tun, peer);
							websWrite(wp, "<div class=\"setting\">\n");
							{
								show_caption(wp, "label", "eoip.wireguard_psk", NULL);
								websWrite(wp, "<input size=\"48\" maxlength=\"48\" name=\"%s\" value=\"%s\" />\n", temp, nvram_safe_get(temp));
							}
							websWrite(wp, "</div>\n");
						}
						websWrite(wp, "</div>\n");
						websWrite(wp, "<script type=\"text/javascript\">\n//<![CDATA[\n");
						websWrite(wp, "show_layer_ext(this, 'idpsk%d_peer%d',%s);\n", tun, peer, nvram_nmatch("1", "oet%d_usepsk%d", tun, peer) ? "true" : "false");
						websWrite(wp, "show_layer_ext(this, 'idendpoint%d_peer%d',%s);\n", tun, peer, nvram_nmatch("1", "oet%d_endpoint%d", tun, peer) ? "true" : "false");
						websWrite(wp,
							  "document.write(\"<input class=\\\"button\\\" type=\\\"button\\\" name=\\\"del_peer_button\\\" value=\\\"\" + eoip.wireguard_delpeer + \"\\\" onclick=\\\"del_peer(this.form,%d,%d)\\\" />\");\n",
							  tun, peer);
						websWrite(wp, "//]]>\n</script>\n");
						websWrite(wp, "</fieldset>\n");
					}
				}

				websWrite(wp, "<div class=\"center\">\n");
				{
					websWrite(wp, "<script type=\"text/javascript\">\n//<![CDATA[\n");
					websWrite(wp,
						  "document.write(\"<input class=\\\"button\\\" type=\\\"button\\\" name=\\\"add_peer_button\\\" value=\\\"\" + eoip.wireguard_addpeer + \"\\\" onclick=\\\"add_peer(this.form,%d)\\\" />\");\n",
						  tun);
					websWrite(wp, "//]]>\n</script>\n");
				}
				websWrite(wp, "</div>\n");
#endif
			}
			websWrite(wp, "</div>\n");
			websWrite(wp, "<div id=\"idlocalip%d\">\n", tun);
			{
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "eoip.localIP", NULL);
					websWrite(wp, "<input type=\"hidden\" name=\"oet%d_local\" value=\"0.0.0.0\"/>\n", tun);
					snprintf(temp, sizeof(temp), "oet%d_local", tun);
					show_ip(wp, NULL, temp, 1, "eoip.localIP");
				}
				websWrite(wp, "</div>\n");
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "eoip.remoteIP", NULL);
					websWrite(wp, "<input type=\"hidden\" name=\"oet%d_rem\" value=\"0.0.0.0\"/>\n", tun);
					snprintf(temp, sizeof(temp), "oet%d_rem", tun);
					show_ip(wp, NULL, temp, 0, "eoip.remoteIP");
				}
				websWrite(wp, "</div>\n");
			}
			websWrite(wp, "</div>\n");
			websWrite(wp, "<div id=\"idl2support%d\">\n", tun);
			{
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "eoip.bridging", NULL);
					snprintf(temp, sizeof(temp), "oet%d_bridged", tun);
					websWrite(wp,
						  "<input class=\"spaceradio\" type=\"radio\" value=\"1\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idbridged%d', false)\" />", temp,
						  (nvram_matchi(temp, 1) ? "checked=\"checked\"" : ""), tun);
					show_caption(wp, NULL, "share.enable", "&nbsp;");
					websWrite(wp,
						  " <input class=\"spaceradio\" type=\"radio\" value=\"0\" name=\"%s\" %s onclick=\"show_layer_ext(this, 'idbridged%d', true)\" />", temp,
						  (nvram_matchi(temp, 0) ? "checked=\"checked\"" : ""), tun);
					show_caption_simple(wp, "share.disable");
				}
				websWrite(wp, "</div>\n");
			}
			websWrite(wp, "</div>\n");
			websWrite(wp, "<div id=\"idbridged%d\">\n", tun);
			{
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "share.ip", NULL);
					websWrite(wp, "<input type=\"hidden\" name=\"oet%d_ipaddr\" value=\"0.0.0.0\"/>\n", tun);
					snprintf(temp, sizeof(temp), "oet%d_ipaddr", tun);
					show_ip(wp, NULL, temp, 0, "share.ip");
				}
				websWrite(wp, "</div>\n");
				websWrite(wp, "<div class=\"setting\">\n");
				{
					show_caption(wp, "label", "share.subnet", NULL);
					websWrite(wp, "<input type=\"hidden\" name=\"oet%d_netmask\" value=\"0.0.0.0\"/>\n", tun);
					snprintf(temp, sizeof(temp), "oet%d_netmask", tun);
					show_ip(wp, NULL, temp, 1, "share.subnet");
				}
				websWrite(wp, "</div>\n");
			}
			websWrite(wp, "</div>\n");
		}
		websWrite(wp, "</div>\n");
		websWrite(wp, "<script type=\"text/javascript\">\n//<![CDATA[\n");
		websWrite(wp, "document.write(\"<input class=\\\"button\\\" type=\\\"button\\\" name=\\\"del_button\\\" value=\\\"\" + eoip.del + \"\\\" onclick=\\\"del_tunnel(this.form,%d)\\\" />\");\n", tun);
		websWrite(wp, "changeproto(document.eop.oet%d_proto, %d, %s, %s);\n", tun, tun, nvram_nget("oet%d_proto", tun), nvram_nget("oet%d_bridged", tun));
		websWrite(wp, "show_layer_ext(document.eop.oet%d_en, 'idoet%d', %s);\n", tun, tun, nvram_nmatch("1", "oet%d_en", tun) ? "true" : "false");
		websWrite(wp, "//]]>\n</script>\n");
		websWrite(wp, "</fieldset><br/>\n");
	}
	websWrite(wp, "<div class=\"center\">\n");
	{
		websWrite(wp, "<script type=\"text/javascript\">\n//<![CDATA[\n");
		websWrite(wp, "document.write(\"<input class=\\\"button\\\" type=\\\"button\\\" name=\\\"add_button\\\" value=\\\"\" + eoip.add + \"\\\" onclick=\\\"add_tunnel(this.form)\\\" />\");\n");
		websWrite(wp, "//]]>\n</script>\n");
	}
	websWrite(wp, "</div>\n");
}
#endif
