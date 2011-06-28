/*!
 * \file mdm_parser_dslam_huawei_ma5600.c Parsers for dslams huawei ma5600.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	<mdm_parser_dslam_huawei_ma5600.h>
#include	<mdm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/
/*!
 * This will try to get timezone information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_huawei_ma5600_timezone(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char *tokens[] = { "The current time zone:  " };
	const char *tmp1;
	const char *tmp2;

	tmp1 = strstr(d->exec_buffer, tokens[0]);
	if(tmp1 != NULL)
	{
		tmp1 += strlen(tokens[0]);

		/* Find end of the value. */
		tmp2 = strchr(tmp1, 13);

		/* Store value. */
		snprintf(
			d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
			"<huawei_ma5600_timezone>%.*s</huawei_ma5600_timezone>",
			(int)(tmp2 - tmp1), tmp1
		);
	} else {
		snprintf(
			d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
			"<huawei_ma5600_timezone/>"
		);
	}
	/* Done. */
	d->exec_buffer_post_len = strlen(d->exec_buffer_post);
	return;
}
/*!
 * This will try to get timezone information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_huawei_ma5600_routes(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	char route_buffer[64];
	int i;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_huawei_ma5600_routes_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_huawei_ma5600_routes_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "huawei_ma5600_routelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_huawei_ma5600_routes_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = strstr(d->exec_buffer, "Destination/Mask");
	if(tmp1 == NULL) {
		goto dslam_huawei_ma5600_routes_done;
	}
	tmp1 = strchr(tmp1, 13);
	if(tmp1 == NULL) {
		goto dslam_huawei_ma5600_routes_done;
	}
	tmp1++;
	while((tmp1 = strchr(tmp1, 13)) != NULL)
	{
		if (strlen(tmp1) < 5) {
			break;
		}
		tmp1+=2;
		node = xmlNewNode(NULL, BAD_CAST "route");
		while(*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, '/');
		snprintf(route_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "destination", BAD_CAST route_buffer);
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		snprintf(route_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "netmask", BAD_CAST route_buffer);
		for(i = 0; i < 5; i++) {
			tmp1 = tmp2;
			while(*tmp1 == 32) tmp1++;
			tmp2 = strchr(tmp1, 32);
		}
		snprintf(route_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "gateway", BAD_CAST route_buffer);
		tmp1 = tmp2;
		while(*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 13);
		snprintf(route_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "interface", BAD_CAST route_buffer);
		/* Add resulting node. */
		xmlAddChild(root_node, node);
	}
	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_huawei_ma5600_routes_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}
/*!
 * This will try to get cpu information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_huawei_ma5600_cpu(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char *tokens[] = { "CPU occupancy: " };
	const char *tmp1;
	const char *tmp2;

	tmp1 = strstr(d->exec_buffer, tokens[0]);
	if(tmp1 != NULL)
	{
		tmp1 += strlen(tokens[0]);

		/* Find end of the value. */
		tmp2 = strchr(tmp1, 13);

		/* Store value. */
		snprintf(
			d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
			"<huawei_ma5600_cpu>%.*s</huawei_ma5600_cpu>",
			(int)(tmp2 - tmp1), tmp1
		);
	} else {
		snprintf(
			d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
			"<huawei_ma5600_cpu/>"
		);
	}
	/* Done. */
	d->exec_buffer_post_len = strlen(d->exec_buffer_post);
	return;
}
/*!
 * This will try to get all service profiles by port
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_huawei_ma5600_service_ports(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char *tokens[] = {
		"index", "vlan", "vlan-attr", "type", "frame-slot", "port",
		"vpi", "vci", "flow-type", "flow-para", "rx", "tx", "state"
	};
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	xmlNodePtr root_node = NULL;
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;
	int i;
	char buffer[64];

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_huawei_ma5600_service_ports_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_huawei_ma5600_service_ports_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "huawei_ma5600_service_ports");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_huawei_ma5600_service_ports_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strstr(d->exec_buffer, "---------");
	if (tmp1 == NULL) {
		goto dslam_huawei_ma5600_service_ports_done;
	}

	tmp1 = strchr(tmp1, 13);
	if (tmp1 == NULL) {
		goto dslam_huawei_ma5600_service_ports_done;
	}
	tmp1 = strstr(tmp1, "---------");
	if (tmp1 == NULL) {
		goto dslam_huawei_ma5600_service_ports_done;
	}

	while((tmp1 = strchr(tmp1, 13)) != NULL)
	{
		tmp1 += 2;
		while(*tmp1 == 32) tmp1++;
		if (*tmp1 == '-') {
			break;
		}
		node = xmlNewNode(NULL, BAD_CAST "service-port");
		for(i = 0; i < 13; i++)
		{
			while(*tmp1 == 32) tmp1++;
			tmp2 = strchr(tmp1, 32);
			tmp3 = strchr(tmp1, 13);
			if (tmp2 > tmp3) {
				tmp2 = tmp3;
			}
			snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(node, NULL, BAD_CAST tokens[i], BAD_CAST buffer);
			tmp1 = tmp2;
		}
		/* Add resulting node. */
		xmlAddChild(root_node, node);
	}
	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_huawei_ma5600_service_ports_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
