# Makefile.am for OCF RAs
#
# Author: Andrew Beekhof
# Copyright (C) 2008 Andrew Beekhof
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST		= $(ocf_SCRIPTS)

ocfdir		    = @OCF_RA_DIR@/pacemaker

ocf_SCRIPTS	     =  ClusterMon 	\
			controld	\
			Dummy		\
			HealthCPU	\
			HealthSMART	\
			o2cb		\
			ping		\
			pingd		\
			Stateful	\
			SysInfo		\
			SystemHealth

if BUILD_XML_HELP
man7_MANS = $(ocf_SCRIPTS:%=ocf_pacemaker_%.7)

%.xml:	%
	echo Building $< manpage
	OCF_FUNCTIONS=/dev/null OCF_ROOT=$(OCF_ROOT_DIR) sh $(top_srcdir)/extra/resources/$< meta-data | $(XSLTPROC) --nonet --novalid --stringparam command.prefix ocf_pacemaker_ --stringparam variable.prefix OCF_RESKEY_ --param man.vol 7 $(top_srcdir)/xml/ocf-meta2man.xsl - > $@

ocf_pacemaker_%.7:	%.xml
	$(XSLTPROC) $(MANPAGE_XSLT) $(top_builddir)/extra/resources/$<
endif

clean-generic:
	rm -f $(man7_MANS) $(ocf_SCRIPTS:%=%.xml) *~

