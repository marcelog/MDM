/*!
 * \file mdm_parser_dslam_zte_8426.c Parser for dslams zte 8426.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	<mdm_parser_dslam_zte_8426.h>
#include	<mdm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * This will try to get software version from dslam. This is
 * the output needed: "ZXDSL 8426 VERSION 1.0.1 2004/07/09(n)".
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_soft_version(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char *token1 = "ZXDSL 8426";
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char soft_version_buffer[32];
	const char *tmp1;
	const char *tmp2;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_soft_version_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_soft_version_done;
	}
	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_soft_version");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_soft_version_done;
	}
	xmlDocSetRootElement(doc, root_node);

	/* Now name. */
	node = xmlNewNode(NULL, BAD_CAST "soft");
	tmp1 = strstr(d->exec_buffer, token1);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_zte_8426_soft_version_done;
	}
	tmp2 = strchr(tmp1, '\n');
	if(tmp2 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found (2).");
		goto dslam_zte_8426_soft_version_done;
	}
	snprintf(soft_version_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
	/*
	 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
	 * creates a node and a text node separately. They are "attached"
	 * by xmlAddChild() 
	 */
	xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST soft_version_buffer);

	/* Add resulting node. */
	xmlAddChild(root_node, node);

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 0);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_8426_soft_version_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get slots from dslam. In this model you only have 1 slot.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_show_slots(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_show_slots_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_slots_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_slotlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_slots_done;
	}
	xmlDocSetRootElement(doc, root_node);
	node = xmlNewNode(NULL, BAD_CAST "slot");
	
	xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST "1" );
	xmlNewChild(node, NULL, BAD_CAST "type", BAD_CAST "xDSL" );
	xmlNewChild(node, NULL, BAD_CAST "enabled", BAD_CAST "yes" );
	xmlNewChild(node, NULL, BAD_CAST "availability", BAD_CAST "available" );

	/* Add resulting node. */
	xmlAddChild(root_node, node);

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_zte_8426_show_slots_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get ethernet information from dslam. In this model you
 * have 10M and 100M ports.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_show_internet(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *ids[] = { "10M", "100M" };
	char *tokens[] = { "The IP Address is:", "The Mask IP Address is:" };
	int tokenslen[] = { 18, 23 };
	char *tokensnames[] = { "ip", "netmask" };
	int i;
	int j;
	const char *tmp1;
	const char *tmp2;
	char internet_buffer[32];

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_show_slots_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_slots_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_internetlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_slots_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 2; i++)
	{
		node = xmlNewNode(NULL, BAD_CAST "internet");
		xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST ids[i] );
		for(j = 0; j < 2; j++)
		{
			/* Find token. */
			tmp1 = strstr(tmp1, tokens[j]);
			tmp1 += tokenslen[j];
	
			/* Advance pointer to value. */
			while(*tmp1 == 32 || *tmp1 == '\t') tmp1++;
	
			/* Find end of the value. */
			tmp2 = strchr(tmp1, 10);
	
			/* Store value. */
			snprintf(internet_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokensnames[j], BAD_CAST internet_buffer
			);
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
dslam_zte_8426_show_slots_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all routes from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_get_routes(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokensnames[] = { "destination", "netmask", "gateway", "interface" };
	char *interfaces[] = { "eth0", "fec0", "lo0" };
	int i;
	int j;
	const char *tmp1;
	const char *tmp2;
	char route_buffer[32];

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_get_routes_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_routes_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_routelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_routes_done;
	}
	xmlDocSetRootElement(doc, root_node);
	for(i = 0; i < 3; i++)
	{
		tmp1 = d->exec_buffer;
		while((tmp1 = strstr(tmp1, interfaces[i])) != NULL)
		{
			node = xmlNewNode(NULL, BAD_CAST "route");
			/* Get last cr. This will give us the current line. */
			tmp2 = tmp1;
			while(*tmp1 != 13) tmp1--;
			tmp1++;
			/* Skip No. field */
			while(*tmp1 == 32) tmp1++;
			while(*tmp1 != 32) tmp1++;

			/* Get tokens. */
			for(j = 0; j < 4; j++)
			{
				while(*tmp1 == 32) tmp1++;
				tmp2 = tmp1;
				while(*tmp2 != 32 && *tmp2 != 10) tmp2++;
				snprintf(route_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
				xmlNewChild(
					node, NULL, BAD_CAST tokensnames[j], BAD_CAST route_buffer
				);
				tmp1 = tmp2 + 1;
			}
			/* Next line. */
			tmp1 = strchr(tmp2, 13);
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
dslam_zte_8426_get_routes_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all atm pvc from DSLAM.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_get_all_pvc(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char pvc_buffer[16];
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	int i = 0;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_get_all_pvc_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_all_pvc_done;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_pvclist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_all_pvc_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = strstr(d->exec_buffer, "VCI");
	if(tmp1 != NULL)
	{
		for(i = 0; i < 24; i++)
		{
			/* Get Slot, we only have 1 slot in this model. */
			node = xmlNewNode(NULL, BAD_CAST "pvc");
			xmlNewChild(node, NULL, BAD_CAST "slot", BAD_CAST "1");
			
			/* Get Port. */
			tmp1 = strchr(tmp1, 13) + 1;
			while(*tmp1 == 32 || *tmp1 == 9) tmp1++;
			tmp2 = strchr(tmp1, 32);
			snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST "port", BAD_CAST pvc_buffer
			);

			/* Get VPI. */
			tmp1 = tmp2;
			while(*tmp1 == 32 || *tmp1 == 9) tmp1++;
			tmp2 = strchr(tmp1, 32);
			snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST "vpi", BAD_CAST pvc_buffer
			);

			/* Get VCI. */
			tmp1 = tmp2;
			while(*tmp1 == 32 || *tmp1 == 9) tmp1++;
			tmp2 = strchr(tmp1, 32);
			snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST "vci", BAD_CAST pvc_buffer
			);
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
dslam_zte_8426_get_all_pvc_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get mac information from dslam. In this model you
 * have 10M and 100M ports.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_get_macs(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *ids[] = { "10M", "100M" };
	char *tokens[] = { "Ethernet Mac address is: " };
	int tokenslen[] = { 25 };
	char *tokensnames[] = { "mac" };
	int i;
	const char *tmp1;
	const char *tmp2;
	char mac_buffer[32];

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_show_slots_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_slots_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_maclist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_slots_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 2; i++)
	{
		node = xmlNewNode(NULL, BAD_CAST "mac");
		xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST ids[i] );
		/* Find token. */
		tmp1 = strstr(tmp1, tokens[0]);
		tmp1 += tokenslen[0];

		/* Advance pointer to value. */
		while(*tmp1 == 32 || *tmp1 == '\t') tmp1++;
	
		/* Find end of the value. */
		tmp2 = strchr(tmp1, 10);
	
		/* Store value. */
		snprintf(mac_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST tokensnames[0], BAD_CAST mac_buffer
		);
		tmp1 = tmp2;

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
dslam_zte_8426_show_slots_done:
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
dslam_zte_8426_get_service_profiles(
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
		goto dslam_zte_8426_get_service_profiles_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_service_profiles_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_serviceprofile_list");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_service_profiles_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strstr(d->exec_buffer, "-------");
	while((tmp1 = strchr(tmp1, '(')) != NULL)
	{
		node = xmlNewNode(NULL, BAD_CAST "serviceprofile");
		tmp1++;
		tmp2 = strchr(tmp1, ')');

		/* Store value. */
		snprintf(profile_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST profile_buffer );

		tmp1 = tmp2 + 1;
		while(*tmp1 == 32) tmp1++;
		tmp2 = strchr(tmp1, 10);

		/* Store value. */
		snprintf(profile_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST profile_buffer );

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
dslam_zte_8426_get_service_profiles_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to show ports from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_show_ports(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;
	int i;
	int j;
	char ports_buffer[32];
	char *tokensnames[] = {
		"id", "type", "coding", "serviceprofile", "ratedownload", "rateupload"
	};
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_show_ports_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_ports_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_portlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_show_ports_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = strstr(d->exec_buffer, "UpCurRate");

	for(i = 0; i < 24; i++)
	{
		node = xmlNewNode(NULL, BAD_CAST "port");
		tmp1 = strchr(tmp1, 13) + 1;
		for(j = 0; j < 6; j++)
		{
			tmp2 = strchr(tmp1, 32);
			tmp3 = strchr(tmp1, 13);
			if(tmp2 > tmp3)
				tmp2 = tmp3;
			/* Store value. */
			snprintf(ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokensnames[j], BAD_CAST ports_buffer
			);
			while(*tmp2 == 32) tmp2++;
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
dslam_zte_8426_show_ports_done:
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
dslam_zte_8426_nop(
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
dslam_zte_8426_create_service_profile(
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
dslam_zte_8426_delete_service_profile(
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
dslam_zte_8426_reboot(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	d->exec_buffer_post_len = 0;
	return;
}

/*!
 * This will try to get service profile.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_zte_8426_get_service_profile(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"AtucConfRateMode = ",
		"AtucConfTargetSnrMgn = ",
		"AtucConfMaxSnrMgn = ",
		"AtucConfMinSnrMgn = ",
		"AtucChanConfFastMaxTxRate = ",
		"AtucChanConfInterleaveMaxTxRate = ",
		"AtucChanConfMaxInterleaveDelay = ",
		"AturConfTargetSnrMgn = ",
		"AturConfMaxSnrMgn = ",
		"AturConfMinSnrMgn = ",
		"AturChanConfFastMaxTxRate = ",
		"AturChanConfInterleaveMaxTxRate = ",
		"AturChanConfMaxInterleaveDelay = ",
		"adslLineConfProfileRowStatus = "
	};
	char *tokensnames[] = {
		"AtucConfRateMode",
		"AtucConfTargetSnrMgn",
		"AtucConfMaxSnrMgn",
		"AtucConfMinSnrMgn",
		"AtucChanConfFastMaxTxRate",
		"AtucChanConfInterleaveMaxTxRate",
		"AtucChanConfMaxInterleaveDelay",
		"AturConfTargetSnrMgn",
		"AturConfMaxSnrMgn",
		"AturConfMinSnrMgn",
		"AturChanConfFastMaxTxRate",
		"AturChanConfInterleaveMaxTxRate",
		"AturChanConfMaxInterleaveDelay",
		"adslLineConfProfileRowStatus"
	};
	int tokenslen[] = { 19, 23, 20, 20, 28, 34, 33, 23, 20, 20, 28, 34, 33, 31 };
	int i;
	const char *tmp1;
	const char *tmp2;
	char prof_buffer[32];

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_zte_8426_get_service_profile_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_service_profile_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "zte_8426_serviceprofile");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_zte_8426_get_service_profile_done;
	}
	xmlDocSetRootElement(doc, root_node);
	for(i = 0; i < 14; i++)
	{
		tmp1 = strstr(d->exec_buffer, tokens[i]);
		if(tmp1 == NULL)
			break;
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 10);
		snprintf(prof_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
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
dslam_zte_8426_get_service_profile_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
