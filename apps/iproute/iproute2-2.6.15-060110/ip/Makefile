IPOBJ=ip.o ipaddress.o iproute.o iprule.o \
    rtm_map.o iptunnel.o ipneigh.o ipntable.o iplink.o \
    ipmaddr.o ipmonitor.o ipmroute.o ipprefix.o \
    ipxfrm.o xfrm_state.o xfrm_policy.o xfrm_monitor.o

RTMONOBJ=rtmon.o

ALLOBJ=$(IPOBJ) $(RTMONOBJ)
SCRIPTS=ifcfg rtpr routel routef
TARGETS=ip
#TARGETS+=rtmon

all: $(TARGETS) $(SCRIPTS)

ip: $(IPOBJ) $(LIBNETLINK) $(LIBUTIL)

rtmon: $(RTMONOBJ) $(LIBNETLINK)

install: all
	$(STRIP) $(TARGETS)
	install -m 0755 $(TARGETS) $(DESTDIR)$(SBINDIR)
#	install -m 0755 $(SCRIPTS) $(DESTDIR)$(SBINDIR)

clean:
	rm -f $(ALLOBJ) $(TARGETS)

