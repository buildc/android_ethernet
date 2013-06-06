
#include <libethernet.h>

extern int ipv4NetmaskToPrefixLength(in_addr_t mask);
extern in_addr_t prefixLengthToIpv4Netmask(int prefix_length);

static int duv060_ethernet_enableInterface(char *ethDev)
{
	int result = 0;

	/* Enable power */

	/* Enable interface */
	result = ifc_enable(ethDev);
	if (result < 0)
	{
		printf("duv060_ethernet_enableInterface::ifc_enable failed!\n");
		goto finish;
	}
	
	/* Disbale dhcp */
	result = dhcp_stop(ethDev);
	if (result < 0)
	{
		printf("duv060_ethernet_enableInterface::dhcp_stop failed!\n");
		goto finish;
	}

finish:
	
	return result;
}

static int duv060_ethernet_disableInterface(char *ethDev)
{
	int result = 0;

	/* Disable interface */
	result = ifc_disable(ethDev);
	
	if (result < 0)
	{
		printf("duv060_ethernet_disableInterface::ifc_disable failed!\n");
		goto finish;
	}

	/* Disbale dhcp */
	result = dhcp_stop(ethDev);
	if (result < 0)
	{
		printf("duv060_ethernet_disableInterface::dhcp_stop failed!\n");
		goto finish;
	}
	
	/* Disable power */
	
finish:

	return result;
}

static int duv060_ethernet_getInterfaceState(char *ethDev)
{
	int		result = -1;
	int		fd = 0;
	int		fd_state = 0;
	char	cmd_buf[128] = {0};
	char	dev_buf[64] = {0};

	/* Find ethDev */
	sprintf(dev_buf, "/sys/class/net/%s/operstate", ethDev);
	fd = open(dev_buf, O_RDONLY);
	if (fd < 0)
	{
		result = -1;
		printf("duv060_ethernet_getInterfaceState::%s not found!\n", ethDev);
		goto finish;
	}
	close(fd);

	/* Check ethDev state */
	sprintf(cmd_buf, "/system/bin/netcfg | grep '%s' | sed 's/.*DOWN.*//g' > /eth.state", ethDev);
	system(cmd_buf);
	
	struct stat st;
	stat("/eth.state", &st);
	if (st.st_size <= 10)
		result = 0;
	else
		result = 1;
		
	remove("/eth.state");

finish:

	return result;
}

static int duv060_ethernet_doDhcp(char *ethDev, char *outIP, char *outNetmask, char *outGateway, char *outDns1, char *outDns2, char *outServer)
{
	int			result = 0;
	uint32_t	lease;
	uint32_t	prefixLength;
	char		prop_name[32];

	/* Disbale dhcp */
	result = dhcp_stop(ethDev);
	if (result < 0)
	{
		printf("duv060_ethernet_doDhcp::dhcp_stop failed!\n");
		goto finish;
	}

	/* Request dhcp */
	result = dhcp_do_request(ethDev, outIP, outGateway, &prefixLength, outDns1, outDns2, outServer, &lease);
	if (result < 0)
	{
		printf("duv060_ethernet_doDhcp::dhcp_do_request failed!\n");
		goto finish;
	}

	/* Get netmask */
	snprintf(prop_name, sizeof(prop_name), "dhcp.%s.mask", ethDev);
	property_get(prop_name, outNetmask, NULL);

	/* Set net.dns1 */
	if (strlen(outDns1) > 0)
		property_set("net.dns1", outDns1);

	/* Set net.dns2 */
	if (strlen(outDns2) > 0)
		property_set("net.dns2", outDns2);

finish:

	return result;
}

static int duv060_ethernet_configureIP(char *ethDev, char *inIP, char *inNetmask, char *inGateway, char *inDns1, char *inDns2)
{
	int			result = 0;
	in_addr_t	address = 0;
	in_addr_t	netmask;
	uint32_t	prefixLength = 0;
	in_addr_t	gateway = 0;
	in_addr_t	dns1 = 0;
	in_addr_t	dns2 = 0;

	if (inIP && strlen(inIP) > 0)
		address = inet_addr(inIP);
	if (inGateway && strlen(inGateway) > 0)
		gateway = inet_addr(inGateway);
	if (inDns1 && strlen(inDns1) > 0)
		dns1 = inet_addr(inDns1);
	if (inDns2 && strlen(inDns2) > 0)
		dns2 = inet_addr(inDns2);
	if (inNetmask && strlen(inNetmask) > 0)
	{
		netmask = inet_addr(inNetmask);
		prefixLength = ipv4NetmaskToPrefixLength(netmask);
	}

	/* Disbale dhcp */
	result = dhcp_stop(ethDev);
	if (result < 0)
	{
		printf("div060_ethernet_configureIP::ifc_disable failed!\n");
		goto finish;
	}
	
	/* Set ip */
	result = ifc_configure(ethDev, address, prefixLength, gateway, dns1, dns2);
	if (result < 0)
	{
		printf("div060_ethernet_configureIP::ifc_configure failed!\n");
		goto finish;
	}

	/* Set net.dns1 */
	if (inDns1 && strlen(inDns1) > 0)
		property_set("net.dns1", inDns1);

	/* Set net.dns2 */
	if (inDns2 && strlen(inDns2) > 0)
		property_set("net.dns2", inDns2);

finish:

	return result;
}

static int duv060_ethernet_configureDNS(char *ethDev, char *inDns1, char *inDns2)
{
	/* Set net.dns1 */
	if (inDns1 && strlen(inDns1) > 0)
		property_set("net.dns1", inDns1);

	/* Set net.dns2 */
	if (inDns2 && strlen(inDns2) > 0)
		property_set("net.dns2", inDns2);
		
	return 0;
}

static int duv060_ethernet_getIP(char *ethDev, char *outIP, char *outNetmask)
{
	int				result = 0;
	char			buf_addr[32];
	char			buf_netmask[32];
	struct in_addr	addr;
	struct in_addr	netmask;
	int				prefixLength;
	int				defaultRoute;
	
	ifc_init();

	result = ifc_get_info(ethDev, &(addr.s_addr), &prefixLength, NULL);
	if (result < 0)
	{
		printf("ifc_get_info::ifc_get_addr failed!\n");
		goto finish;
	}

	strcpy(buf_addr, inet_ntoa(addr));
	strcpy(outIP, buf_addr);
	
	netmask.s_addr = prefixLengthToIpv4Netmask(prefixLength);
	strcpy(buf_netmask, inet_ntoa(netmask));
	strcpy(outNetmask, buf_netmask);

finish:

	ifc_close();

	return result;
}

static int duv060_ethernet_getDNS(char *ethDev, char *outDns1, char *outDns2)
{
	if (outDns1)
		property_get("net.dns1", outDns1, NULL);
	if (outDns2)
		property_get("net.dns2", outDns2, NULL);
	
	return 0;
}

static int duv060_ethernet_checkConnect(char *ethDev)
{
	int		result = -1;
	int		fd = 0;
	char	dev_buf[64] = {0};
	char	buf[64] = {0};

	/* Find ethDev */
	sprintf(dev_buf, "/sys/class/net/%s/carrier", ethDev);
	fd = open(dev_buf, O_RDONLY);
	if (fd >= 0)
	{
		read(fd, buf, sizeof(buf));
		if (strcmp(buf, "0\n") == 0)
			result = 0;
		else
			result = 1;

		close(fd);
	}
	
	return result;
}

static int duv060_ethernet_close(hw_device_t *dev)
{
	free(dev);
	return 0;
}

/*
 * Generic device handling
 */
static int duv060_ethernet_open(const hw_module_t* module, const char* name, hw_device_t** device)
{
	struct duv060_ethernet_device_t	*dev = malloc(sizeof(struct duv060_ethernet_device_t));
	memset(dev, 0, sizeof(*dev));

	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t*)module;
	dev->common.close = duv060_ethernet_close;

	dev->enableInterface = duv060_ethernet_enableInterface;
	dev->disableInterface = duv060_ethernet_disableInterface;
	dev->getInterfaceState = duv060_ethernet_getInterfaceState;
	dev->doDhcp = duv060_ethernet_doDhcp;
	dev->doDhcpRenew = duv060_ethernet_doDhcp;
	dev->configureIP = duv060_ethernet_configureIP;
	dev->configureDNS = duv060_ethernet_configureDNS;
	dev->getIP = duv060_ethernet_getIP;
	dev->getDNS = duv060_ethernet_getDNS;
	dev->checkConnect = duv060_ethernet_checkConnect;

	*device = (hw_device_t*)dev;

	return 0;
}

static struct hw_module_methods_t duv060_ethernet_module_methods = 
{
    .open = duv060_ethernet_open,
};

struct duv060_ethernet_module_t HAL_MODULE_INFO_SYM = 
{
	.common = 
	{
		.tag = HARDWARE_MODULE_TAG, 
		.version_major = 1, 
		.version_minor = 0, 
		.id = DUV060_ETHERNET_MODULE_ID, 
		.name = "DUV060 ETHERNET HW HAL", 
		.author = "DLI", 
		.methods = &duv060_ethernet_module_methods, 
	},
};
