/*!
 * \file mdm_parser_dslam_zte_9xxx.c Parsers for dslams zte 9000 series.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	<mdm_parser_dslam_zte_9xxx.h>
#include	<mdm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * This will try to get slots from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_show_slots(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tmp1;
	char *tmp2;
	char *tmp3;
	char slot_buffer[32];
	int i;
	char *tokensnames[] = {
		"id", "type", "state", "cardtype", "softversion", "portnum", "porttype"
	};
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_show_slots_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_slots_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_slotlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_slots_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strchr(d->exec_buffer, '-');
	tmp1 = strchr(tmp1, 10) + 1;
	while((tmp1 = strchr(tmp1, 10)) != NULL)
	{
		tmp1++;
		node = xmlNewNode(NULL, BAD_CAST "slot");
		for(i = 0; i < 7; i++)
		{
			tmp2 = strchr(tmp1, 32);
			tmp3 = strchr(tmp1, 13);
			if(tmp3 < tmp2)
				tmp2 = tmp3;
			snprintf(slot_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(node, NULL, BAD_CAST tokensnames[i], BAD_CAST slot_buffer);
			tmp1 = tmp2;
			while(*tmp1 == 32) tmp1++;
			if((tmp1 - tmp2) > 15)
			{
				i++;
				xmlNewChild(
					node, NULL, BAD_CAST tokensnames[i], BAD_CAST "N/A"
				);
			}
			
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
dslam_zte_9xxx_show_slots_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to service profiles from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_service_profiles(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	char profile_buffer[32];

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_service_profiles_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_service_profiles_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_serviceprofilelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_service_profiles_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strstr(d->exec_buffer, "-------");
	while((tmp1 = strchr(tmp1, 10)) != NULL)
	{
		node = xmlNewNode(NULL, BAD_CAST "serviceprofile");
		tmp1++;
		tmp2 = strchr(tmp1, 13);

		/* Store value. */
		snprintf(profile_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST profile_buffer );

		tmp1 = tmp2 + 1;

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
dslam_zte_9xxx_get_service_profiles_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get software version from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_soft_version(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = { "name   : ", "state  : ", "size   : " };
	char *tokensnames[] = { "name", "state", "size" } ;
	int tokenslen[] = { 9, 9, 9 };
	char *ids[] = { "Main", "Back" };
	int i;
	int j;
	char soft_version_buffer[64];
	const char *tmp1;
	const char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_soft_version_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_soft_version_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_soft_version");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_soft_version_done;
	}
	xmlDocSetRootElement(doc, root_node);
	
	for(i = 0, tmp1 = d->exec_buffer; i < 2; i++)
	{
		/* Now name. */
		node = xmlNewNode(NULL, BAD_CAST "soft");
		xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST ids[i]);
		for(j = 0; j < 3; j++)
		{
			tmp1 = strstr(tmp1, tokens[j]);
			if(tmp1 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |%s| not found.", tokens[j]);
				goto dslam_zte_9xxx_soft_version_done;
			}
			tmp1 += tokenslen[j];
			tmp2 = strchr(tmp1, ' ');
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |sp| not found.");
				goto dslam_zte_9xxx_soft_version_done;
			}
			snprintf(soft_version_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokensnames[j], BAD_CAST soft_version_buffer
			);
		}
		/* Add resulting node. */
		xmlAddChild(root_node, node);
	}

	tmp1 = strstr(d->exec_buffer, "Boot version name   : ");
	if(tmp1 != NULL)
	{
		tmp1 += 22;
		tmp2 = strchr(tmp1, 32);
		snprintf(soft_version_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			root_node, NULL, BAD_CAST "bootversion", BAD_CAST soft_version_buffer
		);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_soft_version_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all pvc for a port, in this model you can only have
 * 8 pvc's.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_port_pvc(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char pvc_buffer[64];
	const char *tmp1;
	const char *tmp2;
	int i;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_port_pvc_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_port_pvc_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_pvclist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_port_pvc_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strstr(d->exec_buffer, "------");
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_zte_9xxx_port_pvc_done;
	}
	tmp1 = strchr(tmp1, 10);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_zte_9xxx_port_pvc_done;
	}
	
	for(i = 0; i < 8; i++)
	{
		/* Now name. */
		node = xmlNewNode(NULL, BAD_CAST "pvc");
		tmp1++;
		tmp2 = strchr(tmp1, '/');
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "slot", BAD_CAST pvc_buffer);

		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "port", BAD_CAST pvc_buffer);

		tmp1 = strstr(tmp2, "PVC") + 3;
		tmp2 = strchr(tmp1, 32);
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST pvc_buffer);

		tmp1 = tmp2 + 1;
		while(*tmp2 == 32) tmp2++;
		tmp1 = tmp2;
		tmp2 = strchr(tmp1, 32);
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "vpi", BAD_CAST pvc_buffer);

		tmp1 = tmp2 + 1;
		while(*tmp2 == 32) tmp2++;
		tmp1 = tmp2;
		tmp2 = strchr(tmp1, 32);
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "vci", BAD_CAST pvc_buffer);

		tmp1 = tmp2 + 1;
		while(*tmp2 == 32) tmp2++;
		tmp1 = tmp2;
		tmp2 = strchr(tmp1, 32);
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "status", BAD_CAST pvc_buffer);
/*
		tmp1 = tmp2 + 1;
		while(*tmp2 == 32) tmp2++;
		tmp1 = tmp2;
		tmp3 = strchr(tmp1, 10);
		if(tmp2 != tmp3)
		{
			tmp2 = tmp3;
			snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(node, NULL, BAD_CAST "description", BAD_CAST pvc_buffer);
			tmp1 = tmp2;
		}
*/
		tmp1 = strchr(tmp2, 10) + 1;
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
dslam_zte_9xxx_port_pvc_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get dslam uptime.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_uptime(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"sysUpTime      : ", " day, ", " hour, ", " minute, "
	};
	char *tokensnames[] = { "days", "hours", "minutes", "seconds" };
	int tokenslen[] = { 17, 6, 7, 9 };
	int i;
	char uptime_buffer[64];
	const char *tmp1;
	const char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_uptime_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_uptime_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_uptime");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_uptime_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = d->exec_buffer;
	xmlNewChild(root_node, NULL, BAD_CAST "years", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "months", BAD_CAST "0");
	for(i = 0; i < 4; i++)
	{
		tmp1 = d->exec_buffer;
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_zte_9xxx_uptime_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, ' ');
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp| not found.");
			goto dslam_zte_9xxx_uptime_done;
		}
		snprintf(uptime_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST uptime_buffer
		);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_uptime_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all ports from a slot.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_show_slot_ports(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokensnames[] = {
		"admin-status", "link-status", "up", "down", "service-profile"
	};
	int i;
	int j;
	char slot_ports[64];
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_show_slot_ports_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_slot_ports_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_portlistfull");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_slot_ports_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strstr(d->exec_buffer, "------");
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_zte_9xxx_show_slot_ports_done;
	}
	tmp1 = strchr(tmp1, 13);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_zte_9xxx_show_slot_ports_done;
	}

	for(i = 0; i < 48; i++)
	{
		/* Now name. */
		node = xmlNewNode(NULL, BAD_CAST "port");
		tmp1++;
		tmp2 = strchr(tmp1, '/');
		snprintf(slot_ports, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "slot", BAD_CAST slot_ports);

		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		snprintf(slot_ports, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "port", BAD_CAST slot_ports);

		for(j = 0; j < 5; j++)
		{
			tmp1 = tmp2 + 1;
			while(*tmp2 == 32) tmp2++;
			tmp1 = tmp2;
			tmp2 = strchr(tmp1, 32);
			tmp3 = strchr(tmp1, 13);
			if((tmp2 > tmp3) || (tmp2 == NULL))
				tmp2 = tmp3;
			snprintf(slot_ports, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(node, NULL, BAD_CAST tokensnames[j], BAD_CAST slot_ports);
		}
		tmp1 = tmp2 + 1;
		
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
dslam_zte_9xxx_show_slot_ports_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get SCB Information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_show_scb(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"Card Name                :  ",
		"Card Status              :  ",
		"Mini Card Type           :  ",
		"Switch Devices           :  ",
		"SCB postion              :  ",
		"                         :  ",
		"Swap permit              :  ",
		"CPU  Usage               :  ",
		"Memory  Usage            :  "
	};
	char *tokensnames[] = { 
		"name", "status", "minicard-type",
		"switch-devices", "position", "sync",
		"swap-permit", "cpu", "memory"
	};
	int tokenslen[] = { 28, 28, 28, 28, 28, 28, 28, 28, 28 };
	int i;
	char scb_buffer[64];
	const char *tmp1;
	const char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_show_scb_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_scb_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_scb");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_scb_done;
	}
	xmlDocSetRootElement(doc, root_node);

	for(i = 0; i < 9; i++)
	{
		tmp1 = strstr(d->exec_buffer, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_zte_9xxx_show_scb_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		if(tmp2 == NULL)
		{
			tmp2 = tmp1 + strlen(d->exec_buffer);
		} else
			tmp2--;
		snprintf(scb_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST scb_buffer
		);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_show_scb_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get slot Information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_show_slot(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"Configured Card Type     :  ",
		"Run Card Type            :  ",
		"Card HardVer             :  ",
		"Card Status              :  ",
		"Switch Devices           :  ",
		"Link to master SCB       :  ",
		"Loop Check Interval      :  ",
		"CPU  Usage               :  ",
	};
	char *tokensnames[] = { 
		"configured-type", "run-type", "card-hardver",
		"card-status", "switch-devices", "link-master-scb",
		"loopcheck-interval", "cpu"
	};
	int tokenslen[] = { 28, 28, 28, 28, 28, 28, 28, 28 };
	int i;
	char slot_buffer[64];
	const char *tmp1;
	const char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_show_slot_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_slot_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_slot");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_slot_done;
	}
	xmlDocSetRootElement(doc, root_node);

	for(i = 0; i < 8; i++)
	{
		tmp1 = strstr(d->exec_buffer, tokens[i]);
		if(tmp1 == NULL)
			continue;
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		if(tmp2 == NULL)
		{
			tmp2 = tmp1 + strlen(d->exec_buffer);
		} else
			tmp2--;
		snprintf(slot_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST slot_buffer
		);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_show_slot_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get port information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_show_port(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"LineConfProfile     : ",
		"AlarmConfProfile    : ",
		"PMConfPMSF(config)                       : ",
		"PMState(actual)                          : ",
		"TransAtucConfig                          :",
		"TransAtucActual                          :"
	};
	char *tokensnames[] = { 
		"serviceprofile", "alarmprofile", "pmconfpmsf",
		"pmstate", "transatucconfig", "transatucactual"
	};
	int tokenslen[] = { 22, 22, 43, 43, 42, 43 };
	int i;
	int j;
	int port_bufferlen;
	char port_buffer[128];
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_show_port_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_port_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_portprofile");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_port_done;
	}
	xmlDocSetRootElement(doc, root_node);

	for(i = 0; i < 6; i++)
	{
		tmp1 = strstr(d->exec_buffer, tokens[i]);
		if(tmp1 == NULL)
			continue;
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		if(i == 5) {
			tmp2 = tmp1 + strlen(d->exec_buffer);
		} else if(i == 4) {
			tmp1 = tmp2 + 2;
			tmp2 = strchr(tmp1, ':');
			while(*tmp2 != 13) tmp2--;
		} else {
			tmp3 = strchr(tmp1, 32);
			if(tmp3 < tmp2) 
			{
				tmp2 = tmp3;
			}
		}
		if(*tmp1 == 13 || *tmp1 == 32)
			tmp1++;
		snprintf(port_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		port_bufferlen = strlen(port_buffer);
		for(j = 0; j < port_bufferlen; j++)
			if(port_buffer[j] == 13 || port_buffer[j] == 10)
				port_buffer[j] = 32;
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST port_buffer
		);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_show_port_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get running config information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_show_running_config(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tmpbuffer;
	int tmpbufferlen;
		
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_show_running_config_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_running_config_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_running_config");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_show_running_config_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmpbufferlen = strlen(d->exec_buffer) + 1;
	tmpbuffer = MDM_MALLOC(tmpbufferlen);
	if(tmpbuffer == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error allocating memory.");
		goto dslam_zte_9xxx_show_running_config_done;
	}
	memcpy(tmpbuffer, d->exec_buffer, tmpbufferlen);
	/*
	 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
	 * creates a node and a text node separately. They are "attached"
	 * by xmlAddChild() 
	 */
	xmlNewChild(root_node, NULL, BAD_CAST "config", BAD_CAST tmpbuffer);
	MDM_MFREE(&tmpbuffer);

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_show_running_config_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/**
 * Dummy.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_configure_timezone(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	d->exec_buffer_post_len = 0;
	return;
}

/**
 * Dummy.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_create_service_profile(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	d->exec_buffer_post_len = 0;
	return;
}

/**
 * Dummy.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_delete_service_profile(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	d->exec_buffer_post_len = 0;
	return;
}

/**
 * Dummy.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_configure_service_profile(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	d->exec_buffer_post_len = 0;
	return;
}

/**
 * Dummy.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_nop(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	d->exec_buffer_post_len = 0;
	return;
}

/*!
 * This will try to get ntp information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_ntp(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"NTP Status : ",
		"Request Interval : ",
		"Time Zone : ",
		"Server(configured) : ",
		"Stratum : ",
		"Clock Status : "
	};
	char *tokensnames[] = { 
		"status", "interval", "timezone", "server", "stratum", "clockstatus"
	};
	int tokenslen[] = { 13, 19, 13, 21, 10, 15 };
	int i;
	char ntp_buffer[64];
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;
	int rlen = strlen(d->exec_buffer);
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_ntp_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_ntp_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_ntp");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_ntp_done;
	}
	xmlDocSetRootElement(doc, root_node);

	for(i = 0; i < 6; i++)
	{
		tmp1 = strstr(d->exec_buffer, tokens[i]);
		if(tmp1 == NULL)
			continue;
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		tmp3 = strchr(tmp1, 32);
		if(tmp3 < tmp2) 
		{
			tmp2 = tmp3;
		}
		if(tmp2 == NULL)
		{
			tmp2 = tmp1 + rlen;
		}

		snprintf(ntp_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST ntp_buffer
		);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_get_ntp_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get info for a service profile.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_service_profile(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"AtucChanConfFastMinTxRate: ",
		"AtucChanConfInterMinTxRate: ",
		"AtucChanConfFastMaxTxRate: ",
		"AtucChanConfInterMaxTxRate: ",
		"AtucChanConfMaxInterDelay: ",
		"AturChanConfFastMinTxRate: ",
		"AturChanConfInterMinTxRate: ",
		"AturChanConfFastMaxTxRate: ",
		"AturChanConfInterMaxTxRate: ",
		"AturChanConfMaxInterDelay : ",
		"ConfProfileLineType : "
	};
	char *tokensnames[] = { 
		"AtucChanConfFastMinTxRate",
		"AtucChanConfInterMinTxRate",
		"AtucChanConfFastMaxTxRate",
		"AtucChanConfInterMaxTxRate",
		"AtucChanConfMaxInterDelay",
		"AturChanConfFastMinTxRate",
		"AturChanConfInterMinTxRate",
		"AturChanConfFastMaxTxRate",
		"AturChanConfInterMaxTxRate",
		"AturChanConfMaxInterDelay",
		"ConfProfileLineType"
	};
	int tokenslen[] = { 27, 28, 27, 28, 27, 27, 28, 27, 28, 28, 22 };
	int i;
	char prof_buffer[64];
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;
	int rlen = strlen(d->exec_buffer);
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_service_profile_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_service_profile_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_profile");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_service_profile_done;
	}
	xmlDocSetRootElement(doc, root_node);

	for(i = 0; i < 11; i++)
	{
		tmp1 = strstr(d->exec_buffer, tokens[i]);
		if(tmp1 == NULL)
			continue;
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		tmp3 = strchr(tmp1, 32);
		if(tmp3 < tmp2) 
		{
			tmp2 = tmp3;
		}
		if(tmp2 == NULL)
		{
			tmp2 = tmp1 + rlen;
		}

		snprintf(prof_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST prof_buffer
		);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_get_service_profile_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
