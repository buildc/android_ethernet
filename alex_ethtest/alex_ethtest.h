
#ifndef	__ALEX_ETHTEST_H__
#define	__ALEX_ETHTEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libethernet.h>

int		doDhcp(struct duv060_ethernet_device_t *dev, char *ethDev);
int		doDhcpRenew(struct duv060_ethernet_device_t *dev, char *ethDev);
int		setIP(struct duv060_ethernet_device_t *dev, char *ethDev);
int		getStatus(struct duv060_ethernet_device_t *dev, char *ethDev);

#endif	/* __ALEX_ETHTEST_H__ */
