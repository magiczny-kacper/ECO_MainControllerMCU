#ifndef ETHERNETREQUEST_H_
#define ETHERNETREQUEST_H_

typedef enum{
	ETHERNET_REQUEST_TIME_UPDATE,
} EthernetRequest_t;

void EthernetTask_Request (EthernetRequest_t eRequest);

#endif /* ETHERNETREQUEST_H_ */
