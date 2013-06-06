
#ifndef ANDROID_LIBETHERNET_H
#define	ANDROID_LIBETHERNET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <hardware/hardware.h>
#include <netutils/ifc.h>
#include <netutils/dhcp.h>
#include <cutils/properties.h>

__BEGIN_DECLS

#define DUV060_ETHERNET_MODULE_ID		"libethernet"
#define	DUV060_ETHERNET_MODULE_NAME		"duv060_ethernet"

struct duv060_ethernet_module_t
{
	struct hw_module_t	common;
};

struct duv060_ethernet_device_t
{
	struct hw_device_t	common;

	int		(* enableInterface)(char *ethDev);
	int		(* disableInterface)(char *ethDev);
	int		(* getInterfaceState)(char *ethDev);
	int		(* doDhcp)(char *ethDev, char *outIP, char *outNetmask, char *outGateway, char *outDns1, char *outDns2, char *outServer);
	int		(* doDhcpRenew)(char *ethDev, char *outIP, char *outNetmask, char *outGateway, char *outDns1, char *outDns2, char *outServer);
	int		(* configureIP)(char *ethDev, char *inIP, char *inNetmask, char *inGateway, char *inDns1, char *inDns2);
	int		(* configureDNS)(char *ethDev, char *inDns1, char *inDns2);
	int		(* getIP)(char *ethDev, char *outIP, char *outNetmask);
	int		(* getDNS)(char *ethDev, char *outDns1, char *outDns2);
	int		(* checkConnect)(char *ethDev);
};

/** convenience API for opening and closing a device */

static inline int libethernet_open(const struct hw_module_t *module, struct duv060_ethernet_device_t **dev)
{
	return module->methods->open(module, NULL, (struct hw_device_t **)dev);
}

static inline int libethernet_close(struct duv060_ethernet_device_t *dev)
{
	return dev->common.close(&dev->common);
}

static inline int libethernet_enableInterface(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	return dev->enableInterface(ethDev);
}

static inline int libethernet_disableInterface(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	return dev->disableInterface(ethDev);
}

static inline int libethernet_doDhcp(struct duv060_ethernet_device_t *dev, char *ethDev, char *outIP, char *outNetmask, char *outGateway, char *outDns1, char *outDns2, char *outServer)
{
	return dev->doDhcp(ethDev, outIP, outNetmask, outGateway, outDns1, outDns2, outServer);
}

static inline int libethernet_doDhcpRenew(struct duv060_ethernet_device_t *dev, char *ethDev, char *outIP, char *outNetmask, char *outGateway, char *outDns1, char *outDns2, char *outServer)
{
	return dev->doDhcp(ethDev, outIP, outNetmask, outGateway, outDns1, outDns2, outServer);
}

static inline int libethernet_configureIP(struct duv060_ethernet_device_t *dev, char *ethDev, char *inIP, char *inNetmask, char *inGateway, char *inDns1, char *inDns2)
{
	return dev->configureIP(ethDev, inIP, inNetmask, inGateway, inDns1, inDns2);
}

static inline int libethernet_configureDNS(struct duv060_ethernet_device_t *dev, char *ethDev, char *inDns1, char *inDns2)
{
	return dev->configureDNS(ethDev, inDns1, inDns2);
}

static inline int libethernet_getIP(struct duv060_ethernet_device_t *dev, char *ethDev, char *outIP, char *outNetmask)
{
	return dev->getIP(ethDev, outIP, outNetmask);
}

static inline int libethernet_getDNS(struct duv060_ethernet_device_t *dev, char *ethDev, char *outDns1, char *outDns2)
{
	return dev->getDNS(ethDev, outDns1, outDns2);
}

static inline int libethernet_checkConnect(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	return dev->checkConnect(ethDev);
}

static inline int libethernet_getInterfaceState(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	return dev->getInterfaceState(ethDev);
}

__END_DECLS

#endif /* ANDROID_LIBETHERNET_H */
