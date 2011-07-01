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
			if(tmp3 != NULL && tmp3 < tmp2) {
				tmp2 = tmp3;
			}
			snprintf(slot_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(node, NULL, BAD_CAST tokensnames[i], BAD_CAST slot_buffer);
			tmp1 = tmp2;
			if (tmp2 == NULL) {
				break;
			}
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
      if (tmp2 == NULL) {
         break;
      }

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
		"admin-status", "link-status", "up", "down", "coding", "type", "service-profile"
	};
	char *tokensnames2[] = {
		"admin-status", "link-status", "up", "down", "service-profile"
	};
	char **dotokens;

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
	if (strstr(d->exec_buffer, "LineCoding") != NULL) {
		dotokens = tokensnames;
		i = 7;
	} else {
		dotokens = tokensnames2;
		i = 5;
	}
	tmp1 = strchr(tmp1, 13);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_zte_9xxx_show_slot_ports_done;
	}

	while(strchr(tmp1, 13) != NULL)
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

		for(j = 0; j < i; j++)
		{
			tmp1 = tmp2 + 1;
			while(*tmp2 == 32) tmp2++;
			tmp1 = tmp2;
			tmp2 = strchr(tmp1, 32);
			tmp3 = strchr(tmp1, 13);
			if((tmp2 > tmp3) || (tmp2 == NULL))
				tmp2 = tmp3;
			snprintf(slot_ports, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(node, NULL, BAD_CAST dotokens[j], BAD_CAST slot_ports);
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

void
dslam_zte_9xxx_get_port_mac(
		mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char buffer[64];
	char *tokensnames[] = { "device", "mac", "vlan", "pvc", "macnumber" };
	char *tmp1;
	char *tmp2;
	char *tmp3;
	int i;
	int j;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_port_mac_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_port_mac_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_port_mac");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_port_mac_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	while((tmp1 = strstr(tmp1, "MAC NUMBER")) != NULL) {
		tmp1 += 12;
		for (i = 0; i < 6; i++) {
			node = xmlNewNode(NULL, BAD_CAST "port");
			for (j = 0; j < 5; j++) {
				while(*tmp1 == 9 || *tmp1 == 32 || *tmp1 == 13 || *tmp1 == 10) tmp1++;
				tmp2 = strchr(tmp1, 32);
				tmp3 = strchr(tmp1, 13);
				if (tmp3 == NULL) {
					tmp3 = d->exec_buffer + d->exec_buffer_len;
				}
				if (tmp2 == NULL) {
					tmp2 = tmp3;
				}
				if (tmp2 > tmp3) {
					tmp2 = tmp3;
				}
				snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
				xmlNewChild(
					node, NULL, BAD_CAST tokensnames[j], BAD_CAST buffer
				);
				tmp1 = tmp2;
			}
			/* Add resulting node. */
			xmlAddChild(root_node, node);
		}
	}
	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_get_port_mac_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}
/*!
 * This will get all routes in dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_routes(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char buffer[64];
	char *tmp1;
	char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_routes_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_routes_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_routelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_routes_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strstr(d->exec_buffer, "---");
	if (tmp1 == NULL) {
		goto dslam_zte_9xxx_get_routes_done;
	}
	while ((tmp1 = strchr(tmp1, 13)) != NULL) {
		node = xmlNewNode(NULL, BAD_CAST "route");
		tmp1+=2;

		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "destination", BAD_CAST buffer);
		tmp1 = tmp2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "netmask", BAD_CAST buffer);
		tmp1 = tmp2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "gateway", BAD_CAST buffer);
		tmp1 = tmp2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "status", BAD_CAST buffer);
		tmp1 = tmp2;

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
dslam_zte_9xxx_get_routes_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}
/*!
 * This will get full port information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_port_full(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"Interface           : ",
		"pvid                : ",
		"name                : ",
		"pvid PVC1           : ",
		"pvid PVC2           : ",
		"pvid PVC3           : ",
		"pvid PVC4           : ",
		"pvid PVC5           : ",
		"pvid PVC6           : ",
		"pvid PVC7           : ",
		"pvid PVC8           : ",
		"psvid PVC1          : ",
		"psvid PVC2          : ",
		"psvid PVC3          : ",
		"psvid PVC4          : ",
		"psvid PVC5          : ",
		"psvid PVC6          : ",
		"psvid PVC7          : ",
		"psvid PVC8          : ",
		"IRL PVC1(kbps)      : ",
		"IRL PVC2(kbps)      : ",
		"IRL PVC3(kbps)      : ",
		"IRL PVC4(kbps)      : ",
		"IRL PVC5(kbps)      : ",
		"IRL PVC6(kbps)      : ",
		"IRL PVC7(kbps)      : ",
		"IRL PVC8(kbps)      : ",
		"AdminStatus         : ",
		"AtucLoopType        : ",
		"LinkStatus(ADSL)    : ",
		"ifType              : ",
		"ifMtu               : ",
		"CosPriority PVC1    : ",
		"CosPriority PVC2    : ",
		"CosPriority PVC3    : ",
		"CosPriority PVC4    : ",
		"CosPriority PVC5    : ",
		"CosPriority PVC6    : ",
		"CosPriority PVC7    : ",
		"CosPriority PVC8    : ",
		"Trap                : ",
		"MaxMacLearn         : ",
		"Fast-leave          : ",
		"Group Limit         : ",
		"Group Band          : ",
		"rmon history        : ",
		"pppoe-plus          : ",
		"dhcp option82       : ",
		"dhcp option82 sub-option: ",
		"dhcp option82 sub-option remote-id: ",
		"atm pvc1            : ",
		"atm pvc2            : ",
		"atm pvc3            : ",
		"atm pvc4            : ",
		"atm pvc5            : ",
		"atm pvc6            : ",
		"atm pvc7            : ",
		"atm pvc8            : ",
		"atm status pvc1     : ",
		"atm status pvc2     : ",
		"atm status pvc3     : ",
		"atm status pvc4     : ",
		"atm status pvc5     : ",
		"atm status pvc6     : ",
		"atm status pvc7     : ",
		"atm status pvc8     : ",
		"qos profile         : ",
		"PVC1 trust          : ",
		"PVC2 trust          : ",
		"PVC3 trust          : ",
		"PVC4 trust          : ",
		"PVC5 trust          : ",
		"PVC6 trust          : ",
		"Queue 1 Buffer size : ",
		"Queue 2 Buffer size : ",
		"Queue 3 Buffer size : ",
		"Queue 4 Buffer size : ",
		"adsl profile        : ",
		"adsl alarm-profile  : ",
		"Fast-leave          : ",
		"adsl linetype       : ",
		"adsl linecoding     : ",
		"atm pvc             :  ",
		"DuplexSpeed         : ",
		"LinkStatus(Eth)     : "
	};
	char *tokensnames[] = {
		"id", "pvid", "name", "pvid-1", "pvid-2", "pvid-3", "pvid-4", "pvid-5",
		"pvid-6", "pvid-7", "pvid-8", "psvid-1", "psvid-2",  "psvid-3",
		"psvid-4", "psvid-5", "psvid-6", "psvid-7", "psvid-8", "irl-pvc-1", "irl-pvc-2",
		"irl-pvc-3", "irl-pvc-4", "irl-pvc-5", "irl-pvc-6", "irl-pvc-7",
		"irl-pvc-8", "admin-status", "atuc-loop-type", "oper-status",
		"if-type", "if-mtu", "cos-pvc-1", "cos-pvc-2", "cos-pvc-3",
		"cos-pvc-4", "cos-pvc-5", "cos-pvc-6", "cos-pvc-7", "cos-pvc-8",
		"trap", "max-mac-learn", "fast-leave", "group-limit",
		"group-band", "rmon-history", "pppoe-plus", "dhcp-option82",
		"dhcp-sub-option", "dhcp-sub-option-remote-id",
		"atm-pvc-1", "atm-pvc-2", "atm-pvc-3", "atm-pvc-4", "atm-pvc-5",
		"atm-pvc-6", "atm-pvc-7", "atm-pvc-8", "atm-status-pvc-1",
		"atm-status-pvc-2", "atm-status-pvc-3", "atm-status-pvc-4",
		"atm-status-pvc-5", "atm-status-pvc-6", "atm-status-pvc-7",
		"atm-status-pvc-8", "qos-profile", "trust-pvc-1", "trust-pvc-2",
		"trust-pvc-3", "trust-pvc-4", "trust-pvc-5", "trust-pvc-6",
		"queue-buffer-1", "queue-buffer-2", "queue-buffer-3", "queue-buffer-4",
		"adsl-profile", "alarm-profile", "fast-leave", "type", "coding", "pvc",
		"duplex-speed", "link-status"
	};
	int i;
	char buffer[64];
	char *tmp1;
	char *tmp2;
	char *tmp3;

	char *vlantag = "Tag Vlan List:";
	char *vlanuntag = "Untag Vlan List:";

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_port_full_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_port_full_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_port_full");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_port_full_done;
	}
	xmlDocSetRootElement(doc, root_node);
	node = xmlNewNode(NULL, BAD_CAST "port");
	for (i = 0; i < 85; i++) {
		tmp1 = strstr(d->exec_buffer, tokens[i]);
		if (tmp1 == NULL) {
			continue;
		}
		tmp1 += strlen(tokens[i]);
		tmp2 = strchr(tmp1, 32);
		tmp3 = strchr(tmp1, 13);
		if ((tmp3 != NULL && tmp3 < tmp2) || (strstr(tokensnames[i], "trust-pvc") != NULL)) {
			tmp2 = tmp3;
		}
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST tokensnames[i], BAD_CAST buffer);
	}
	tmp1 = strstr(d->exec_buffer, vlantag);
	if (tmp1 != NULL) {
		tmp1 = strchr(tmp1, 13) + 1;
		while((tmp2 = strchr(tmp1, 13)) != NULL) {
			snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			if (strtol(buffer, NULL, 10) == 0) {
				break;
			}
			xmlNewChild(node, NULL, BAD_CAST "vlan-tag", BAD_CAST buffer);
			tmp1 = tmp2 + 1;
		}
	}
	tmp1 = strstr(d->exec_buffer, vlanuntag);
	if (tmp1 != NULL) {
		tmp1 = strchr(tmp1, 13) + 2;
		while((tmp2 = strchr(tmp1, 13)) != NULL) {
			snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			if (strtol(buffer, NULL, 10) == 0) {
				break;
			}
			xmlNewChild(node, NULL, BAD_CAST "vlan-untag", BAD_CAST buffer);
			tmp1 = tmp2 + 1;
		}
	}
	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlAddChild(root_node, node);
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_get_port_full_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;

}

/*!
 * This will get physical port information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_physical_port(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	int i;
	char buffer[64];
	char *tokens[] = {
		"InvVendorID",
/*		"InvVersionNumber    : ", */
		"CurrSnrMgn",
		"CurrAtn",
		"OutputPwr",
		"AttainableRate(kbps)",
		"InvVendorID",
/*		"InvVersionNumber    : ", */
		"CurrSnrMgn",
		"CurrAtn",
		"CurrOutputPwr",
		"AttainableRate(kbps)",
		"adslAtucDMTState",
		"adslAturDMTState",
		"zxAdslAtucPrevSnrMgn",
		"zxAdslAtucPrevAtn",
		"zxAdslAtucPrevAttainableRate",
		"zxAdslAturPrevSnrMgn",
		"zxAdslAturPrevAtn",
		"zxAdslAturPrevAttainableRate"
	};
	char *tokensnames[] = {
		"atuc-vendor", /*"atuc-version",*/
		"atuc-snrmgn", "atuc-atn", "atuc-output", "atuc-attainable",
		"atur-vendor", /*"atur-version",*/ "atur-snrmgn",
		"atur-atn", "atur-output", "atur-attainable",
		"atuc-dmt", "atur-dmt",
		"atuc-prevsnrmgn", "atuc-prevatn", "atuc-prev-attainable",
		"atur-prevsnrmgn", "atur-prevatn", "atur-prev-attainable"
	};
	char *tmp1;
	char *tmp2;
	char *tmp3;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_physical_port_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_physical_port_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_physicalinfo");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_physical_port_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 18; i++)
	{
		tmp3 = strstr(tmp1, tokens[i]);
		if (tmp3 == NULL) {
			continue;
		}
		tmp1 = tmp3 + strlen(tokens[i]);
		while(*tmp1 == 32) tmp1++;
		while(*tmp1 != ':') tmp1++;
		tmp1++;
		while(*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		tmp3 = strchr(tmp1, 13);
		if ((tmp3 != NULL && tmp3 < tmp2)) {
			tmp2 = tmp3;
		}
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST buffer);
		tmp1 = tmp2;
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_get_physical_port_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will get physical port information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_network_info(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	int i;
	char buffer[64];
	char *tokens[] = {
		"MAC address         : ",
		"IP address          : ",
		"Netmask             : ",
		"MAC address         : ",
	};
	char *tokensnames[] = {
		"inband-mac", "outbound-ip", "outbound-mask", "outbound-mac"
	};
	char *tmp1;
	char *tmp2;
	char *tmp3;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_network_info_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_network_info_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_network_info");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_network_info_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 4; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if (tmp1 == NULL) {
			continue;
		}
		tmp1 += strlen(tokens[i]);
		tmp2 = strchr(tmp1, 32);
		tmp3 = strchr(tmp1, 13);
		if ((tmp3 != NULL && tmp3 < tmp2)) {
			tmp2 = tmp3;
		}
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST buffer);
		tmp1 = tmp2;
	}
	tmp1 = strstr(d->exec_buffer, "---");
	if (tmp1 == NULL) {
		goto dslam_zte_9xxx_get_network_info_done;
	}
	while ((tmp1 = strchr(tmp1, 13)) != NULL) {
		node = xmlNewNode(NULL, BAD_CAST "address");
		tmp1 += 2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		if (strstr(buffer, "In")) {
			break;
		}
		xmlNewChild(node, NULL, BAD_CAST "ip", BAD_CAST buffer);
		tmp1 = tmp2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "netmask", BAD_CAST buffer);
		tmp1 = tmp2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "vid", BAD_CAST buffer);
		tmp1 = tmp2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "status", BAD_CAST buffer);
		tmp1 = tmp2;

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
dslam_zte_9xxx_get_network_info_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}
/*!
 * This will get physical channel information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_channel_info(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	int i;
	char buffer[64];
	char *tokens[] = {
		"ChanInterDelay      : ",
		"ChanCurrTxRate(kbps): ",
		"ChanPrevTxRate(kbps): ",
		"ChanCrcBlockLen     : ",
		"ChanInterDelay      : ",
		"ChanCurrTxRate(kbps): ",
		"ChanPrevTxRate(kbps): ",
		"ChanCrcBlockLen     : ",
		"zxAdslAtucChanRsSymbols: ",
		"zxAdslAtucChanRsDepth: ",
		"zxAdslAtucChanRsRedundancy: ",
		"zxAdslAturChanRsSymbols: ",
		"zxAdslAturChanRsDepth: ",
		"zxAdslAturChanRsRedundancy: "
	};
	char *tokensnames[] = {
		"atuc-interdelay", "atuc-currtxrate", "atuc-prevtxrate", "atuc-crclen",
		"atur-interdelay", "atur-currtxrate", "atur-prevtxrate", "atur-crclen",
		"atuc-rssymbols", "atuc-rsdepth", "atuc-rsredundancy",
		"atur-rssymbols", "atur-rsdepth", "atur-rsredundancy"
	};
	char *tmp1;
	char *tmp2;
	char *tmp3;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_channel_info_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_channel_info_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_channel_info");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_channel_info_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 14; i++)
	{
		tmp3 = strstr(d->exec_buffer, tokens[i]);
		if (tmp3 == NULL) {
			continue;
		}
		tmp1 = tmp3;
		tmp1 = strchr(tmp1, 32);
		while(*tmp1 != ':') tmp1++;
		tmp1+=2;
		tmp2 = strchr(tmp1, 32);
		tmp3 = strchr(tmp1, 13);
		if ((tmp3 != NULL && tmp3 < tmp2)) {
			tmp2 = tmp3;
		}
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST buffer);
	}

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_get_channel_info_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}
/*!
 * This will get fans information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_9xxx_get_fans_info(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char buffer[64];
	char *tmp1;
	char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_9xxx_get_fans_info_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_fans_info_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_9xxx_fans_info");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_9xxx_get_fans_info_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	if (strstr(tmp1, "Sorry") != NULL) {
		goto dslam_zte_9xxx_get_fans_info_done;
	}
	do
	{
		tmp1+=2;
		node = xmlNewNode(NULL, BAD_CAST "fan");
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 32);
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST buffer);

		tmp1 = tmp2;
		while (*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 13);
		if (tmp2 == NULL) {
			tmp2 = d->exec_buffer + d->exec_buffer_len;
		}
		snprintf(buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "status", BAD_CAST buffer);
		xmlAddChild(root_node, node);
		tmp1 = tmp2;
	} while((tmp1 = strchr(tmp1, 13)) != NULL);
	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_9xxx_get_fans_info_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
