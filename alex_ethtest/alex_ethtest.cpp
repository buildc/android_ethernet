
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <cutils/log.h>
#include <utils/Timers.h>
#include "alex_ethtest.h"

int main(int argc, char** argv)
{
	int		ret = 0;
	int		err = 0;
	int		enable = 0;
	bool	exit = false;
	char	buf;
	char	ethDev[16];
	
	strcpy(ethDev, "eth0");

	struct duv060_ethernet_module_t	*module;
	struct duv060_ethernet_device_t	*device;
	
	printf("alex_ethtest::start!\r\n");

	err = hw_get_module(DUV060_ETHERNET_MODULE_ID, (hw_module_t const**)&module);
	if (err != 0)
	{
		printf("alex_ethtest::hw_get_module(%s) failed (%s)\n", DUV060_ETHERNET_MODULE_ID, strerror(-err));
		ret = -1;
		goto finish;
	}

	err = libethernet_open(&module->common, &device);
	if (err != 0)
	{
		printf("alex_ethtest::libethernet_open() failed (%s)\n", strerror(-err));
		ret = -1;
		goto finish;
	}

	while (!exit)
	{
		enable = libethernet_getInterfaceState(device, ethDev);
		if (enable == -1)
		{
			printf("alex_ethtest::no %s device!\n", ethDev);
			ret = -1;
			goto finish;
		}
	
		printf("\n+++ alex_ethtest +++\n");
	
		if (enable == 1)	
		{
			printf("[1] Disable eth0\n");
			printf("[2] Enable DHCP\n");
			printf("[3] Renew DHCP\n");
			printf("[4] Set IP\n");
			printf("[5] Get Net Status\n");
			printf("\n");
			printf("[Q] Quit\n");
		}
		else
		{
			printf("[1] Enable eth0\n");
			printf("\n");
			printf("[Q] Quit\n");
		}

		printf("--- alex_ethtest ---\n\n");

		buf = getchar();

		if (enable == 1)
		{
			switch (buf)
			{
			case '1':
				err = libethernet_disableInterface(device, ethDev);
				break;

			case '2':
				err = doDhcp(device, ethDev);
				break;
			
			case '3':
				err = doDhcpRenew(device, ethDev);
				break;
				
			case '4':
				err = setIP(device, ethDev);
				break;
			
			case '5':
				err = getStatus(device, ethDev);
				break;
			
			case 'q':
			case 'Q':
				exit = true;
				goto finish;
				break;
			}
		}
		else
		{
			switch (buf)
			{
			case '1':
				err = libethernet_enableInterface(device, ethDev);
				break;
			
			case 'q':
			case 'Q':
				exit = true;
				goto finish;
				break;
			}
		}
	}

finish:

	if (ret != 0)
		printf("alex_ethtest::failed!\r\n");

	err = libethernet_close(device);
	if (err != 0)
		printf("alex_ethtest::libethernet_close() failed (%s)\n", strerror(-err));

	printf("alex_ethtest::leave!\r\n");

	return ret;
}

int doDhcp(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	int	result = 0;
	
	char		ipaddr[32];
	char		netmask[32];
	char		gateway[32];
	char		dns1[32];
	char		dns2[32];
	char		server[32];

	result = libethernet_doDhcp(dev, ethDev, ipaddr, netmask, gateway, dns1, dns2, server);
	if (result == 0)
	{
		printf("doDhcp::ipaddr = %s\n", ipaddr);
		printf("doDhcp::netmask = %s\n", netmask);
		printf("doDhcp::gateway = %s\n", gateway);
		printf("doDhcp::dns1 = %s\n", dns1);
		printf("doDhcp::dns2 = %s\n", dns2);
		printf("doDhcp::server = %s\n", server);
	}

	return result;
}

int doDhcpRenew(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	int	result = 0;
	
	char		ipaddr[32];
	char		netmask[32];
	char		gateway[32];
	char		dns1[32];
	char		dns2[32];
	char		server[32];

	result = libethernet_doDhcpRenew(dev, ethDev, ipaddr, netmask, gateway, dns1, dns2, server);
	if (result == 0)
	{
		printf("doDhcpRenew::ipaddr = %s\n", ipaddr);
		printf("doDhcpRenew::netmask = %s\n", netmask);
		printf("doDhcpRenew::gateway = %s\n", gateway);
		printf("doDhcpRenew::dns1 = %s\n", dns1);
		printf("doDhcpRenew::dns2 = %s\n", dns2);
		printf("doDhcpRenew::server = %s\n", server);
	}

	return result;
}

int setIP(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	char	ip[32];
	char	netmask[32];
	char	gateway[32];
	char	dns1[32];
	char	dns2[32];

	strcpy(ip, "192.168.1.201");
	strcpy(netmask, "255.255.255.0");
	strcpy(gateway, "192.168.1.1");
	strcpy(dns1, "168.95.192.1");
	strcpy(dns2, "168.95.1.1");
	
	return libethernet_configureIP(dev, ethDev, ip, netmask, gateway, dns1, dns2);
}

int	getStatus(struct duv060_ethernet_device_t *dev, char *ethDev)
{
	int		result = 0;
	char	ip[32];
	char	netmask[32];
	char	dns1[32];
	char	dns2[32];

	result = libethernet_getIP(dev, ethDev, ip, netmask);
	if (result < 0)
	{
		printf("getStatus::linethernet_getIP failed!\n");
		goto finish;
	}
	
	printf("getStatus::ip = %s\n", ip);
	printf("getStatus::netmask = %s\n", netmask);

	result = libethernet_getDNS(dev, ethDev, dns1, dns2);
	if (result < 0)
	{
		printf("getStatus::linethernet_getDNS failed!\n");
		goto finish;
	}

	printf("getStatus::dns1 = %s\n", dns1);
	printf("getStatus::dns2 = %s\n", dns2);
	
	result = libethernet_checkConnect(dev, ethDev);
	if (result < 0)
	{
		printf("getStatus::libethernet_checkConnect failed!\n");
		goto finish;
	}
	
	if (result == 0)
		printf("getStatus::RJ45 out\n");
	else
		printf("getStatus::RJ45 in\n");

finish:
	
	return result;
}

