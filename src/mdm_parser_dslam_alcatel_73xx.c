/*!
 * \file mdm_parser_dslam_alcatel_73xx.c Parser for dslams alcatel 73xx.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	<mdm_parser_dslam_alcatel_73xx.h>
#include	<mdm.h>

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/
/*!
 * This will try to get uptime from dslam (in cents of seconds). This is
 * the output needed: "up-time : 2220186550"
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_uptime(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	long dslam_uptime = 0;
	char uptime_buffer[64];
	char *uptime_start;
	char *uptime_end;
	char *token = "up-time : ";
	int tokenlen = 10;
/*
	long dslam_uptime_years = 0;
	long dslam_uptime_months = 0;
*/
	long dslam_uptime_days = 0;
	long dslam_uptime_hours = 0;
	long dslam_uptime_minutes = 0;
	long dslam_uptime_seconds = 0;
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_uptime_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_uptime_done;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_uptime");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_uptime_done;
	}
	xmlDocSetRootElement(doc, root_node);

	/* Find needed output. */
	uptime_start = strstr(d->exec_buffer, token);
	if(uptime_start == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_alcatel_73xx_uptime_done;
	}
	uptime_start += tokenlen;
	uptime_end = strchr(uptime_start + tokenlen, ' ');
	if(uptime_end == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found (2).");
		goto dslam_alcatel_73xx_uptime_done;
	}

	/* Got what we needed. Translate it. */
	snprintf(uptime_buffer, uptime_end - uptime_start, "%s", uptime_start);
	dslam_uptime = strtol(uptime_buffer, NULL, 10);
	dslam_uptime /= 100;

	/* Get days. */
	dslam_uptime_days = dslam_uptime / 86400;
	dslam_uptime -= (dslam_uptime_days * 86400);

	/* Get hours. */
	dslam_uptime_hours = dslam_uptime / 3600;
	dslam_uptime -= (dslam_uptime_hours * 3600);

	/* Get minutes. */
	dslam_uptime_minutes = dslam_uptime / 60;
	dslam_uptime -= (dslam_uptime_minutes * 60);

	/* Get seconds. */
	dslam_uptime_seconds = dslam_uptime;

	/* 
	 * xmlNewChild() creates a new node, which is "attached" as child node
	 * of root_node node. 
	 */
	xmlNewChild(root_node, NULL, BAD_CAST "years", BAD_CAST "0");
	xmlNewChild(root_node, NULL, BAD_CAST "months", BAD_CAST "0");

	snprintf(uptime_buffer, sizeof(uptime_buffer), "%ld", dslam_uptime_days);
	xmlNewChild(root_node, NULL, BAD_CAST "days", BAD_CAST uptime_buffer);

	snprintf(uptime_buffer, sizeof(uptime_buffer), "%ld", dslam_uptime_hours);
	xmlNewChild(root_node, NULL, BAD_CAST "hours", BAD_CAST uptime_buffer);

	snprintf(uptime_buffer, sizeof(uptime_buffer), "%ld", dslam_uptime_minutes);
	xmlNewChild(root_node, NULL, BAD_CAST "minutes", BAD_CAST uptime_buffer);

	snprintf(uptime_buffer, sizeof(uptime_buffer), "%ld", dslam_uptime_seconds);
	xmlNewChild(root_node, NULL, BAD_CAST "seconds", BAD_CAST uptime_buffer);

	/* Dump the document to a buffer and print it for demonstration purposes. */
	xmlNodeDump(psBuf, doc, root_node, 99, 1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"%s", xmlBufferContent(psBuf)
	);
	d->exec_buffer_post_len = xmlBufferLength(psBuf);

	/* Done. */
dslam_alcatel_73xx_uptime_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get software version from dslam. This is
 * the output needed: "name : " and "err-file-name : ".
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_soft_version(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"index : ", "name : ", "availability : ", "act-status : ",
		"commit-status : ", "download-error : ", "err-file-name : ",
		"err-file-server : ", "auto-activate-error : "
	};
	char *tokensnames[] = {
		"index", "name", "availability", "act-status",
		"commit-status", "download-error", "err-file-name",
		"err-file-server", "auto-activate-error"
	};
	int tokenslen[] = {8, 7, 15, 13, 16, 17, 16, 18, 22};
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
		goto dslam_alcatel_73xx_soft_version_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_soft_version_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_soft_version");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_soft_version_done;
	}
	xmlDocSetRootElement(doc, root_node);
	
	for(i = 0, tmp1 = d->exec_buffer; i < 2; i++)
	{
		/* Now name. */
		node = xmlNewNode(NULL, BAD_CAST "soft");
		for(j = 0; j < 9; j++)
		{
			tmp1 = strstr(tmp1, tokens[j]);
			if(tmp1 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |%s| not found.", tokens[j]);
				goto dslam_alcatel_73xx_soft_version_done;
			}
			tmp1 += tokenslen[j];
			tmp2 = strchr(tmp1, ' ');
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |sp| not found.");
				goto dslam_alcatel_73xx_soft_version_done;
			}
			snprintf(soft_version_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			/*
			 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
			 * creates a node and a text node separately. They are "attached"
			 * by xmlAddChild() 
			 */
			xmlNewChild(
				node, NULL, BAD_CAST tokensnames[j], BAD_CAST soft_version_buffer
			);
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
dslam_alcatel_73xx_soft_version_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get slots from dslam. This is
 * the output needed:
 * lt:1/1/1  nalt-c      yes     no-error               available     0
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_show_slots(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char slots_buffer[64];
	const char *tmp1;
	const char *tmp2;
	char *tmpforln;
	int slot_count = 0;
	int i;
	int j;
	char *tokens[] = {
		"id", "type", "enabled", "error-status", "availability", "restrt-cnt"
	};
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_show_slots_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_slots_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_slotlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_slots_done;
	}
	xmlDocSetRootElement(doc, root_node);
	/* Limit buffer counting slot count. */
	tmpforln = strstr(d->exec_buffer, "slot count : ");
	if(tmpforln == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_alcatel_73xx_show_slots_done;
	}
	tmpforln += 13;
	snprintf(
		slots_buffer, strchr(tmpforln, 13) - tmpforln + 1,
		"%s", tmpforln
	);
	slot_count = strtol(slots_buffer, NULL, 10);
	
	/* Lookup the end of the header. */
	tmp1 = strchr(d->exec_buffer, '+');
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found 2.");
		goto dslam_alcatel_73xx_show_slots_done;
	}

	/* Get all slots info. */
	for(j = 0; j < slot_count; j++)
	{
		/* Find next line. */
		tmp1 = strchr(tmp1, 10);
		node = xmlNewNode(NULL, BAD_CAST "slot");
		for(i = 0; i < 6; i++)
		{
			tmp1++;
			tmp2 = strchr(tmp1, ' ');
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |sp| not found.");
				goto dslam_alcatel_73xx_show_slots_done;
			}
			if(tmp1[2] == ':')
				tmp1 += 3;
			snprintf(slots_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokens[i], BAD_CAST slots_buffer
			);
			/* Neeext. */
			tmp1 = tmp2;
			while(*tmp1 == 32) tmp1++;
			tmp1--;
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
dslam_alcatel_73xx_show_slots_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get ethernet information from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_show_internet(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *ids[] = { "1" };
	char *tokens[] = { "host-ip-address", "/" };
	int tokenslen[] = { 15, 1 };
	char *tokensnames[] = { "ip", "netmask" };
	const char *tmp1;
	const char *tmp2;
	char internet_buffer[64];
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_show_internet_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_internet_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_internetlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_internet_done;
	}
	xmlDocSetRootElement(doc, root_node);

	node = xmlNewNode(NULL, BAD_CAST "internet");
	xmlNewChild(node, NULL, BAD_CAST "id", BAD_CAST ids[0] );

	tmp1 = strstr(d->exec_buffer, tokens[0]);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found 2.");
		goto dslam_alcatel_73xx_show_internet_done;
	}
	tmp1 += tokenslen[0];
	tmp1 = strchr(tmp1, ':');
	tmp1++;

	/* Find end of the value. */
	tmp2 = strchr(tmp1, '/');

	/* Store value. */
	snprintf(internet_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
	xmlNewChild(
		node, NULL, BAD_CAST tokensnames[0], BAD_CAST internet_buffer
	);
	tmp1 = tmp2;
	tmp2 = strchr(tmp1, 13);
	/* Store value. */
	snprintf(internet_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
	xmlNewChild(
		node, NULL, BAD_CAST tokensnames[1], BAD_CAST internet_buffer
	);
	
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
dslam_alcatel_73xx_show_internet_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get information for a specific port.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_show_slot_port(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"if-index : ", "adm-state : ", "opr-state/tx-rate-ds : ", "tx-rate/us : ",
		"tx-rate/ds : ", "max-tx-rate-us : ", "max-tx-rate-ds : ", "inp-up : ",
		"inp-dn : ", "interl-us : ", "interl-ds : ", "cur-op-mode : ",
		"rinit-1d : ", "actual-tps-tc-mode : "
	};
	int tokenslen[] = { 11, 12, 23, 13, 13, 17, 17, 9, 9, 12, 12, 14, 11, 21};
	char *tokensnames[] = {
		"if-index", "adm-state", "opr-state-tx-rate-ds", "tx-rate-us",
		"tx-rate-ds", "max-tx-rate-us", "max-tx-rate-ds", "inp-up",
		"inp-dn", "interl-us", "interl-ds", "cur-op-mode",
		"rinit-1d", "actual-tps-tc-mode"
	};
	const char *tmp1;
	const char *tmp2;
	char slot_port_buffer[64];
	int i;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_show_slot_port_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_slot_port_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_datalineinfo");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_slot_port_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 14; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_alcatel_73xx_show_slot_port_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, ' ');
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp| not found.");
			goto dslam_alcatel_73xx_show_slot_port_done;
		}
		snprintf(slot_port_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST slot_port_buffer
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
dslam_alcatel_73xx_show_slot_port_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all spectrum profiles with its modes.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_show_spectrum_profiles(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = { "configure xdsl spectrum-profile ", "name ", "version " };
	int tokenslen[] = { 32, 5, 8 };
	char *tokensnames[] = { "id", "name", "version"};
	const char *tmp1;
	const char *tmp2;
	char spectrum_prf_buffer[64];
	int i;
	int j;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_show_spectrum_profiles_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_spectrum_profiles_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_spectrumprofilelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_spectrum_profiles_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = d->exec_buffer;
	while((tmp1 = strstr(tmp1, tokens[0])) != NULL)
	{
		node = xmlNewNode(NULL, BAD_CAST "spectrumprofile");
		tmp1 += tokenslen[0];
		tmp2 = strchr(tmp1, ' ');
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp| not found.");
			goto dslam_alcatel_73xx_show_spectrum_profiles_done;
		}
		snprintf(spectrum_prf_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST tokensnames[0], BAD_CAST spectrum_prf_buffer
		);
		tmp1 = tmp2;
		for(i = 1; i < 3; i++)
		{
			tmp1 = strstr(tmp1, tokens[i]);
			if(tmp1 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
				goto dslam_alcatel_73xx_show_spectrum_profiles_done;
			}
			tmp1 += tokenslen[i];
			tmp2 = strchr(tmp1, ' ');
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |sp| not found.");
				goto dslam_alcatel_73xx_show_spectrum_profiles_done;
			}
			snprintf(spectrum_prf_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokensnames[i], BAD_CAST spectrum_prf_buffer
			);
		}
		/* From now on to eol, we have modes. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 13);
		if(tmp2 == NULL)
			tmp2 = d->exec_buffer + strlen(d->exec_buffer);
		j = 0;
		memset(spectrum_prf_buffer, 0, sizeof(spectrum_prf_buffer));
		while(tmp1 < tmp2)
		{
			if(*tmp1 != 32)
			{
				spectrum_prf_buffer[j] = *tmp1;
				j++;
			} else {
				if(strncmp(spectrum_prf_buffer, "no", 2) == 0)
				{
					do
					{
						spectrum_prf_buffer[j] = *tmp1;
						tmp1++;
						j++;
					} while(tmp1 < tmp2 && *tmp1 != 13 && *tmp1 != 32);
				}
				xmlNewChild(
					node, NULL, BAD_CAST "mode", BAD_CAST spectrum_prf_buffer
				);
				j = 0;
				memset(spectrum_prf_buffer, 0, sizeof(spectrum_prf_buffer));
			}
			tmp1++;
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
dslam_alcatel_73xx_show_spectrum_profiles_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all service profiles with its modes.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_show_service_profiles(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"configure xdsl service-profile ", "name ", "version ",
		"ra-mode-down ", "ra-mode-up ",
		"min-bitrate-down ", "min-bitrate-up ",
		"plan-bitrate-down ", "plan-bitrate-up ",
		"max-bitrate-down ", "max-bitrate-up ",
		"max-delay-down ", "max-delay-up "
	};
	int tokenslen[] = { 31, 5, 8, 13, 11, 17, 15, 18, 16, 17, 15, 15, 13 };
	char *tokensnames[] = {
		"id", "name", "version", "ra-mode-down", "ra-mode-up",
		"min-bitrate-down", "min-bitrate-up",
		"plan-bitrate-down", "plan-bitrate-up",
		"max-bitrate-down", "max-bitrate-up",
		"max-delay-down", "max-delay-up"
	};
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;
	char service_prf_buffer[64];
	int nl;
	int i;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_show_service_profiles_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_service_profiles_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_serviceprofilelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_service_profiles_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = d->exec_buffer;
	while((tmp1 = strstr(tmp1, tokens[0])) != NULL)
	{
		node = xmlNewNode(NULL, BAD_CAST "serviceprofile");
		tmp1 += tokenslen[0];
		tmp2 = strchr(tmp1, ' ');
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp| not found.");
			goto dslam_alcatel_73xx_show_service_profiles_done;
		}
		snprintf(service_prf_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST tokensnames[0], BAD_CAST service_prf_buffer
		);
		tmp1 = tmp2;
		/* nl is used to identify new lines, truncated service profiles. */
		nl = 0;
		for(i = 1; (i < 13) && !nl; i++)
		{
			tmp3 = strchr(tmp1, 13);
			tmp2 = strstr(tmp1, tokens[i]);
			if((tmp3 < tmp2) || tmp2 == NULL)
			{
				continue;
			}
			tmp1 = tmp2;
			tmp1 += tokenslen[i];
			if(i == 12)
				tmp2 = strchr(tmp1, 13);
			else
				tmp2 = strchr(tmp1, 32);
			if(tmp2 == NULL)
			{
				tmp2 = strchr(tmp1, 13);
				nl = 1;
			}
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(
					status->status_message, "Token |sp| (%s) not found.", tokens[i]
				);
				goto dslam_alcatel_73xx_show_service_profiles_done;
			}
			snprintf(service_prf_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokensnames[i], BAD_CAST service_prf_buffer
			);
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
dslam_alcatel_73xx_show_service_profiles_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get ports from dslam. This is
 * the output needed:
 * 1/1/1/25  up        up :2053       292        2053       g992-5-a
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_show_ports(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char ports_buffer[64];
	const char *tmp1;
	const char *tmp2;
	char *tmpforln;
	int port_count = 0;
	int i;
	int j;
	char *tokens[] = {
		"id", "adm-state", "opr-state-tx-rate-ds",
		"tx-rate-us", "tx-rate-ds", "cur-op-mode"
	};
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_show_ports_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_ports_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_portstatistics");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_show_ports_done;
	}
	xmlDocSetRootElement(doc, root_node);
	/* Limit buffer counting slot count. */
	tmpforln = strstr(d->exec_buffer, "line count : ");
	if(tmpforln == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_alcatel_73xx_show_ports_done;
	}
	tmpforln += 13;
	snprintf(
		ports_buffer, strchr(tmpforln, 13) - tmpforln + 1,
		"%s", tmpforln
	);
	port_count = strtol(ports_buffer, NULL, 10);
	
	/* Lookup the end of the header. */
	tmp1 = strchr(d->exec_buffer, '+');
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found 2.");
		goto dslam_alcatel_73xx_show_ports_done;
	}

	/* Get all slots info. */
	for(j = 0; j < port_count; j++)
	{
		/* Find next line. */
		tmp1 = strchr(tmp1, 10);
		node = xmlNewNode(NULL, BAD_CAST "port");
		for(i = 0; i < 6; i++)
		{
			tmp1++;
			if(tmp1[3] == ':')
				tmp1 += 4;
			tmp2 = strchr(tmp1, ' ');
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |sp| not found.");
				goto dslam_alcatel_73xx_show_ports_done;
			}
			snprintf(ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokens[i], BAD_CAST ports_buffer
			);
			/* Neeext. */
			tmp1 = tmp2;
			while(*tmp1 == 32) tmp1++;
			tmp1--;
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
dslam_alcatel_73xx_show_ports_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all routes version from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_routes(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = { "default-route " };
	int tokenslen[] = { 14 };
	const char *tmp1;
	const char *tmp2;
	char route_buffer[64];
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_routes_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_routes_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_routelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_routes_done;
	}
	xmlDocSetRootElement(doc, root_node);

	node = xmlNewNode(NULL, BAD_CAST "route");
	xmlNewChild(node, NULL, BAD_CAST "destination", BAD_CAST "0.0.0.0" );
	xmlNewChild(node, NULL, BAD_CAST "netmask", BAD_CAST "0.0.0.0" );

	tmp1 = strstr(d->exec_buffer, tokens[0]);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found 2.");
		goto dslam_alcatel_73xx_get_routes_done;
	}
	tmp1 += tokenslen[0];

	/* Find end of the value. */
	tmp2 = strchr(tmp1, 13);

	/* Store value. */
	snprintf(route_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
	xmlNewChild(node, NULL, BAD_CAST "gateway", BAD_CAST route_buffer);
	xmlNewChild(node, NULL, BAD_CAST "interface", BAD_CAST "1" );
	
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
dslam_alcatel_73xx_get_routes_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get port description information from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_port_description(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char *tokens[] = { "user " };
	int tokenslen[] = { 5 };
	const char *tmp1;
	const char *tmp2;
	
	tmp1 = strstr(d->exec_buffer, tokens[0]);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found 2.");
		return;
	}
	tmp1 += tokenslen[0];

	/* Find end of the value. */
	tmp2 = strchr(tmp1, 13);

	/* Store value. */
	d->exec_buffer_post_len = (int)(tmp2 - tmp1);
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"<alcatel_73xx_portdescription>%.*s</alcatel_73xx_portdescription>",
		d->exec_buffer_post_len, tmp1
	);
	
	/* Done. */
	return;
}

/*!
 * This will try to get port profile from DSLAM.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_port_profile(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char port_profile[64];
	char *tokens[] = {
		"service-profile ", "spectrum-profile ", "\n  ", "transfer-mode "
	};
	int tokenslen[] = { 16, 17, 3, 14 };
	char *tokensnames[] = {
		"spectrumprofile", "serviceprofile", "status", "mode"
	};
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	int i;
	const char *tmp1;
	const char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_port_profile_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_port_profile_done;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_portprofile");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_port_profile_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 4; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_alcatel_73xx_port_profile_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |cr| not found.");
			goto dslam_alcatel_73xx_port_profile_done;
		}
		snprintf(port_profile, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST port_profile
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
dslam_alcatel_73xx_port_profile_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get port data (near end) from DSLAM.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_port_data_near_end(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char port_profile[64];
	char *tokens[] = {
		"rel-cap-occ-up : ", "noise-margin-up : ", "output-power-down : ",
		"sig-attenuation-up : ", "loop-attenuation-up : ", "actual-opmode : ",
		"xtu-c-opmode : ", "ansi-t1413 : ", "etsi-dts : ", "g992-1-a : ",
		"g992-1-b : ", "g992-2-a : ", "g992-3-a : ", "g992-3-b : ",
		"g992-3-l1 : ", "g992-3-l2 : ", "g992-3-am : ", "g992-5-a : ",
		"g992-5-b : ",	"ansi-t1.424 : ", "etsi-ts : ", "itu-g993-1 : ",
		"ieee-802.3ah : ", "g992-5-am : ", "g993-2-8a : ", "g993-2-8b : ",
		"g993-2-8c : ", "g993-2-8d : ", "g993-2-12a : ", "g993-2-12b : ",
		"g993-2-17a : ", "g993-2-30a : ", "actual-psd-down : ",
		"power-mgnt-state : ", "per-bnd-lp-att-up : ", "pr-bnd-sgn-att-up : ",
		"pr-bnd-nois-mg-up : ", "high-freq-up : ", "elect-length : ",
		"time-adv-corr : ", "actual-tps-tc-mode : "
	};
	int tokenslen[] = {
		17, 18, 20, 21, 22, 16, 15, 13, 11, 11, 11, 11, 11, 11, 12, 12, 12, 11,
		11, 14, 10, 13, 15, 12, 12, 12, 12, 12, 13, 13, 13, 13, 18, 19, 20, 20,
		20, 15, 15, 16, 21
	};
	char *tokensnames[] = {
		"rel-cap-occ-up", "noise-margin-up", "output-power-down",
		"sig-attenuation-up", "loop-attenuation-up", "actual-opmode",
		"xtu-c-opmode", "ansi-t1413", "etsi-dts", "g992-1-a",
		"g992-1-b", "g992-2-a", "g992-3-a", "g992-3-b", "g992-3-l1",
		"g992-3-l2", "g992-3-am", "g992-5-a", "g992-5-b",
		"ansi-t1.424", "etsi-ts", "itu-g993-1", "ieee-802.3ah",
		"g992-5-am", "g993-2-8a", "g993-2-8b", "g993-2-8c",
		"g993-2-8d", "g993-2-12a", "g993-2-12b", "g993-2-17a",
		"g993-2-30a", "actual-psd-down", "power-mgnt-state",
		"per-bnd-lp-att-up", "pr-bnd-sgn-att-up", "pr-bnd-nois-mg-up",
		"high-freq-up", "elect-length", "time-adv-corr",
		"actual-tps-tc-mode"
	};
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	int i;
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_port_data_near_end_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_port_data_near_end_done;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_portdatanearend");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_port_data_near_end_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 41; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_alcatel_73xx_port_data_near_end_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 32);
		tmp3 = strchr(tmp1, 13);
		if(tmp2 == NULL && tmp3 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp or cr| not found.");
			goto dslam_alcatel_73xx_port_data_near_end_done;
		}
		if(tmp2 > tmp3)
			tmp2 = tmp3;
		snprintf(port_profile, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST port_profile
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
dslam_alcatel_73xx_port_data_near_end_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get port data (far end) from DSLAM.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_port_data_far_end(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char port_profile[64];
	char *tokens[] = {
		"rel-cap-occ-down : ", "noise-margin-down : ", "output-power-up : ",
		"sig-attenuation-down : ", "loop-attenuation-down : ", 
		"xtu-r-opmode : ", "ansi-t1413 : ", "etsi-dts : ", "g992-1-a : ",
		"g992-1-b : ", "g992-2-a : ", "g992-3-a : ", "g992-3-b : ",
		"g992-3-l1 : ", "g992-3-l2 : ", "g992-3-am : ", "g992-5-a : ",
		"g992-5-b : ",	"ansi-t1.424 : ", "etsi-ts : ", "itu-g993-1 : ",
		"ieee-802.3ah : ", "g992-5-am : ", "g993-2-8a : ", "g993-2-8b : ",
		"g993-2-8c : ", "g993-2-8d : ", "g993-2-12a : ", "g993-2-12b : ",
		"g993-2-17a : ", "g993-2-30a : ", "actual-psd-up : ",
		"pr-bnd-lp-att-dn : ", "pr-bnd-sig-att-dn : ",
		"pr-bnd-nmgn-dn : ", "elect-length-est : ", "time-adv-prp : ",
		"high-freq-dn : "
	};
	int tokenslen[] = {
		19, 20, 18, 23, 24, 15, 13, 11, 11, 11, 11, 11, 11, 12, 12, 12, 11,
		11, 14, 10, 13, 15, 12, 12, 12, 12, 12, 13, 13, 13, 13, 16, 19,
		20, 17, 19, 15, 15
	};
	char *tokensnames[] = {
		"rel-cap-occ-down", "noise-margin-down", "output-power-up",
		"sig-attenuation-down", "loop-attenuation-down", 
		"xtu-r-opmode", "ansi-t1413", "etsi-dts", "g992-1-a",
		"g992-1-b", "g992-2-a", "g992-3-a", "g992-3-b",
		"g992-3-l1", "g992-3-l2", "g992-3-am", "g992-5-a",
		"g992-5-b",	"ansi-t1.424", "etsi-ts", "itu-g993-1",
		"ieee-802.3ah", "g992-5-am", "g993-2-8a", "g993-2-8b",
		"g993-2-8c", "g993-2-8d", "g993-2-12a", "g993-2-12b",
		"g993-2-17a", "g993-2-30a", "actual-psd-up",
		"pr-bnd-lp-att-dn", "pr-bnd-sig-att-dn",
		"pr-bnd-nmgn-dn", "elect-length-est", "time-adv-prp",
		"high-freq-dn"
	};
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	int i;
	const char *tmp1;
	const char *tmp2;
	const char *tmp3;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_port_data_near_end_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_port_data_near_end_done;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_portdatafarend");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_port_data_near_end_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 38; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_alcatel_73xx_port_data_near_end_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 32);
		tmp3 = strchr(tmp1, 13);
		if(tmp2 == NULL && tmp3 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp or cr| not found.");
			goto dslam_alcatel_73xx_port_data_near_end_done;
		}
		if(tmp2 > tmp3)
			tmp2 = tmp3;
		snprintf(port_profile, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST port_profile
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
dslam_alcatel_73xx_port_data_near_end_done:
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
dslam_alcatel_73xx_get_all_pvc(
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

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_all_pvc_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_all_pvc_done;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_pvclist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_all_pvc_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = d->exec_buffer;
	while((tmp1 = strstr(tmp1, "pvc")) != NULL)
	{
		node = xmlNewNode(NULL, BAD_CAST "pvc");

		/* Get Slot. */
		tmp1 += 4;
		tmp2 = strchr(tmp1, '/') + 1;
		tmp2 = strchr(tmp2, '/') + 1;
		tmp2 = strchr(tmp2, '/');
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "slot", BAD_CAST pvc_buffer
		);

		/* Get Port. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, ':');
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "port", BAD_CAST pvc_buffer
		);

		/* Get VPI. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, ':');
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "vpi", BAD_CAST pvc_buffer
		);
		
		/* Get VCI. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 13);
		snprintf(pvc_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "vci", BAD_CAST pvc_buffer
		);
		
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
dslam_alcatel_73xx_get_all_pvc_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get bridge information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_bridge(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char port_profile[64];
	char *tokens[] = {
		"vlan-id ", "pvid "
	};
	int tokenslen[] = { 8, 5 };
	char *tokensnames[] = {
		"vlanid", "pvid"
	};
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	int i;
	const char *tmp1;
	const char *tmp2;

	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_bridge_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_bridge_done;
	}
	
	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_vlan");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_bridge_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 2; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_alcatel_73xx_get_bridge_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |cr| not found.");
			goto dslam_alcatel_73xx_get_bridge_done;
		}
		snprintf(port_profile, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST port_profile
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
dslam_alcatel_73xx_get_bridge_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get backup status information.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_backup_status(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = {
		"disk-space : ", "free-space : ", "download-progress : ",
		"download-error : ", "upload-progress : ",
		"upload-error : ", "auto-activate-error : "
	};
	int tokenslen[] = { 13, 13, 20, 17, 18, 15, 22 };
	char *tokensnames[] = {
		"disk-space", "free-space", "download-progress",
		"download-error", "upload-progress",
		"upload-error", "auto-activate-error"
	};
	const char *tmp1;
	const char *tmp2;
	char backup_status[64];
	int i;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_backup_status_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_backup_status_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_backupstatus");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_backup_status_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	for(i = 0; i < 7; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_alcatel_73xx_get_backup_status_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, ' ');
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp| not found.");
			goto dslam_alcatel_73xx_get_backup_status_done;
		}
		snprintf(backup_status, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST backup_status
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
dslam_alcatel_73xx_get_backup_status_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get databases information (firmware).
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_databases(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokensnames[] = {
		"container", "number", "name", "status", "version", "error"
	};
	const char *tmp1;
	const char *tmp2;
	char database_buffer[64];
	int i;
	int j;
	int dbcount;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_databases_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_databases_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_databaselist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_databases_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = strstr(d->exec_buffer, "database count : ");
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_alcatel_73xx_get_databases_done;
	}
	tmp1 += 17;
	snprintf(database_buffer, strchr(tmp1, 13) - tmp1 + 1, "%s", tmp1);
	dbcount = strtol(database_buffer, NULL, 10);
	
	/* Lookup the end of the header. */
	tmp1 = strchr(d->exec_buffer, '+');
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found 2.");
		goto dslam_alcatel_73xx_get_databases_done;
	}
	for(j = 0; j < dbcount; j++)
	{
		node = xmlNewNode(NULL, BAD_CAST "database");
		tmp1 = strchr(tmp1, 10) + 1;
		for(i = 0; i < 6; i++)
		{
			/* Advance pointer. */
			while(*tmp1 == 32) tmp1++;

			/* Find end of token. */
			tmp2 = strchr(tmp1, ' ');
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |sp| not found.");
				goto dslam_alcatel_73xx_get_databases_done;
			}
			snprintf(database_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			/*
			 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
			 * creates a node and a text node separately. They are "attached"
			 * by xmlAddChild() 
			 */
			xmlNewChild(
				node, NULL, BAD_CAST tokensnames[i], BAD_CAST database_buffer
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
dslam_alcatel_73xx_get_databases_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all vlans with vpi and vci.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_slot_vlans(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	char slot_vlans_buffer[64];
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_slot_vlans_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_slot_vlans_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_vlanlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_slot_vlans_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	
	while((tmp1 = strstr(tmp1, "configure interface port vlan-port:")) != NULL)
	{
		node = xmlNewNode(NULL, BAD_CAST "vlan");
		tmp1 += 35;
		/* Get Slot. */
		tmp2 = strchr(tmp1, '/') + 1;
		tmp2 = strchr(tmp2, '/') + 1;
		tmp2 = strchr(tmp2, '/');
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "slot", BAD_CAST slot_vlans_buffer
		);

		/* Get Port. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, ':');
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "port", BAD_CAST slot_vlans_buffer
		);

		/* Get VPI. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, ':');
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "vpi", BAD_CAST slot_vlans_buffer
		);
		
		/* Get VCI. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, ':');
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "vci", BAD_CAST slot_vlans_buffer
		);

		/* Get VLAN. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "vlan", BAD_CAST slot_vlans_buffer
		);

		/* Get admin status. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		if(tmp2 == NULL)
			continue;
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "admin-status", BAD_CAST slot_vlans_buffer
		);

		/* Get level. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		if(tmp2 == NULL)
			continue;
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "level", BAD_CAST slot_vlans_buffer
		);

		/* Get value. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 13);
		if(tmp2 == NULL)
			continue;
		snprintf(slot_vlans_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "value", BAD_CAST slot_vlans_buffer
		);
		
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
dslam_alcatel_73xx_get_slot_vlans_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all ports from a slot with description.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_load_ports(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	char load_ports_buffer[64];
	int i;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_load_ports_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_load_ports_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_portlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_load_ports_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	
	for(i = 0; i < 48; i++)
	{
		node = xmlNewNode(NULL, BAD_CAST "port");
		tmp1 = strstr(tmp1, "configure interface port xdsl-line:");
		tmp1 += 35;
		/* Get Slot. */
		tmp2 = strchr(tmp1, '/') + 1;
		tmp2 = strchr(tmp2, '/') + 1;
		tmp2 = strchr(tmp2, '/');
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "slot", BAD_CAST load_ports_buffer
		);

		/* Get Port. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "port", BAD_CAST load_ports_buffer
		);

		/* Get admin status. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		if(tmp2 == NULL)
			continue;
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "admin-status", BAD_CAST load_ports_buffer
		);

		/* Get description. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		tmp1 = tmp2 + 1;
		if(i != 47)
		{
			tmp2 = strchr(tmp1, 13);
			if(tmp2 == NULL)
				continue;
		} else {
			tmp2 = d->exec_buffer + d->exec_buffer_len;
		}
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "description", BAD_CAST load_ports_buffer
		);
		
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
dslam_alcatel_73xx_load_ports_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all alarm types.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_alarm_types(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	int rlen;
	char alarm_types_buffer[64];
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_alarm_types_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_alarm_types_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_alarmtypelist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_alarm_types_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	rlen = strlen(d->exec_buffer);
	MDM_LOGDEBUG("XXXXXXXXXXXX d->exec_buffer len: %ld", rlen);
	while(tmp1 < (d->exec_buffer + rlen))
	{
		if(strchr(tmp1, '+') == NULL)
			break;
		node = xmlNewNode(NULL, BAD_CAST "alarmtype");
		tmp2 = strchr(tmp1, 32);
		snprintf(alarm_types_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "name", BAD_CAST alarm_types_buffer
		);
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
dslam_alcatel_73xx_get_alarm_types_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get alarm count from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_alarm_count(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	char *tokens[] = { "count : " };
	int tokenslen[] = { 8 };
	const char *tmp1;
	tmp1 = strstr(d->exec_buffer, tokens[0]);
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		return;
	}
	tmp1 += tokenslen[0];

	/* Store value. */
	snprintf(
		d->exec_buffer_post, MDM_DEVICE_EXEC_BUFFER_POST_MAX_LEN,
		"<alcatel_73xx_alarmcount>%s</alcatel_73xx_alarmcount>",
		tmp1
	);
	d->exec_buffer_post_len = strlen(d->exec_buffer_post);
	/* Done. */
	return;
}

/*!
 * This will try to get eths from dslam.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_eth_list(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char eth_buffer[64];
	const char *tmp1;
	const char *tmp2;
	char *tmpforln;
	int eth_count = 0;
	int i;
	int j;
	char *tokens[] = {
		"id", "admin-status", "oper-status", "speed", "type", "duplex"
	};
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_eth_list_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_eth_list_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_ethlist");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_eth_list_done;
	}
	xmlDocSetRootElement(doc, root_node);
	/* Limit buffer counting slot count. */
	tmpforln = strstr(d->exec_buffer, "port count : ");
	if(tmpforln == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found 2.");
		goto dslam_alcatel_73xx_get_eth_list_done;
	}
	tmpforln += 13;
	snprintf(eth_buffer, strchr(tmpforln, 13) - tmpforln + 1, "%s", tmpforln);
	eth_count = strtol(eth_buffer, NULL, 10);
	
	/* Lookup the end of the header. */
	tmp1 = strchr(d->exec_buffer, '+');
	if(tmp1 == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Token not found.");
		goto dslam_alcatel_73xx_get_eth_list_done;
	}

	/* Get all slots info. */
	for(j = 0; j < eth_count; j++)
	{
		/* Find next line. */
		tmp1 = strchr(tmp1, 10);
		tmp1++;
		node = xmlNewNode(NULL, BAD_CAST "eth");
		for(i = 0; i < 6; i++)
		{
			tmp2 = strchr(tmp1, ' ');
			if(tmp2 == NULL)
			{
				status->status = MDM_OP_ERROR;
				sprintf(status->status_message, "Token |sp| not found.");
				goto dslam_alcatel_73xx_get_eth_list_done;
			}
			snprintf(eth_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
			xmlNewChild(
				node, NULL, BAD_CAST tokens[i], BAD_CAST eth_buffer
			);
			/* Neeext. */
			tmp1 = tmp2;
			while(*tmp1 == 32) tmp1++;
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
dslam_alcatel_73xx_get_eth_list_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get statistics for an eth port.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_get_eth_statistics(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	char ethstats_buffer[64];
	const char *tmp1;
	const char *tmp2;
	int i;
	char *tokens[] = {
		"port : ", "in-octets : ", "in-ucast-pkts : ",
		"in-addr-ucast-pkts : ", "in-discard-pkts : ", "in-err-pkts : ",
		"pkts-unknown-proto : ", "out-octets : ", "out-ucast-pkts : ", 
		"out-addr-ucast-pkts : ", "out-discard-pkts : ", "out-err-pkts : "
	};
	int tokenslen[] = { 7, 12, 16, 21, 18, 14, 21, 13, 17, 22, 19, 15 };
	char *tokensnames[] = {
		"id", "in-octets", "in-ucast-pkts",
		"in-addr-ucast-pkts", "in-discard-pkts", "in-err-pkts",
		"pkts-unknown-proto", "out-octets", "out-ucast-pkts", 
		"out-addr-ucast-pkts", "out-discard-pkts", "out-err-pkts"
	};
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_eth_statistics_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_eth_statistics_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_ethstatistics");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_eth_statistics_done;
	}
	xmlDocSetRootElement(doc, root_node);

	/* Lookup the end of the header. */
	tmp1 = d->exec_buffer;

	/* Find next line. */
	node = xmlNewNode(NULL, BAD_CAST "eth");
	for(i = 0; i < 12; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |%s| not found.", tokens[i]);
			goto dslam_alcatel_73xx_get_eth_statistics_done;
		}
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, ' ');
		if(tmp2 == NULL)
		{
			status->status = MDM_OP_ERROR;
			sprintf(status->status_message, "Token |sp| not found.");
			goto dslam_alcatel_73xx_get_eth_statistics_done;
		}
		snprintf(ethstats_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST tokensnames[i], BAD_CAST ethstats_buffer
		);
		/* Neeext. */
		tmp1 = tmp2;
	}
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
dslam_alcatel_73xx_get_eth_statistics_done:
	if(doc != NULL)
		xmlFreeDoc(doc);
	if(psBuf != NULL)
		xmlBufferFree(psBuf);
	return;
}

/*!
 * This will try to get all detail from all ports in all slots.
 * \param d Device descriptor.
 * \param status Result of the operation.
 */
void
dslam_alcatel_73xx_load_ports_all(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlNodePtr node = NULL;
	xmlBufferPtr psBuf = NULL;
	const char *tmp1;
	const char *tmp2;
	char load_ports_buffer[64];
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_load_ports_all_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_load_ports_all_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_portlistfull");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_load_ports_all_done;
	}
	xmlDocSetRootElement(doc, root_node);
	tmp1 = d->exec_buffer;
	
	while((tmp1 = strstr(tmp1, "configure xdsl line ")) != NULL)
	{
		node = xmlNewNode(NULL, BAD_CAST "port");
		tmp1 += 20;
		/* Get Slot. */
		tmp2 = strchr(tmp1, '/') + 1;
		tmp2 = strchr(tmp2, '/') + 1;
		tmp2 = strchr(tmp2, '/');
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "slot", BAD_CAST load_ports_buffer
		);

		/* Get Port. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "port", BAD_CAST load_ports_buffer
		);

		/* Get service profile. */
		tmp1 = strchr(tmp2 + 1, 32) + 1;
		tmp2 = strchr(tmp1, 32);
		if(tmp2 == NULL)
			continue;
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "service-profile", BAD_CAST load_ports_buffer
		);

		/* Get spectrum profile. */
		tmp1 = strchr(tmp2 + 1, 32) + 1;
		tmp2 = strchr(tmp1, 32);
		if(tmp2 == NULL)
			continue;
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "spectrum-profile", BAD_CAST load_ports_buffer
		);

		/* Get admin status. */
		tmp1 = tmp2 + 1;
		tmp2 = strchr(tmp1, 32);
		if(tmp2 == NULL)
			continue;
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "admin-status", BAD_CAST load_ports_buffer
		);

		/* Get transfer mode. */
		tmp1 = strchr(tmp2 + 1, 32) + 1;
		tmp2 = strchr(tmp1, 13);
		if(tmp2 == NULL)
			continue;
		snprintf(load_ports_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		xmlNewChild(
			node, NULL, BAD_CAST "transfer-mode", BAD_CAST load_ports_buffer
		);

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
dslam_alcatel_73xx_load_ports_all_done:
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
dslam_alcatel_73xx_configure_timezone(
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
dslam_alcatel_73xx_get_ntp(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	xmlDocPtr doc = NULL; /* document pointer */
	xmlNodePtr root_node = NULL;
	xmlBufferPtr psBuf = NULL;
	char *tokens[] = { "server-ip-addr ", "  ", "timezone-offset " };
	char *tokensnames[] = { "server", "status", "timezone" };
	int tokenslen[] = { 15, 2, 16 };
	int i;
	char ntp_buffer[64];
	const char *tmp1;
	const char *tmp2;
	
	/* Create target buffer. */
	psBuf = xmlBufferCreate();
	if(psBuf == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating buffer for xml.");
		goto dslam_alcatel_73xx_get_ntp_done;
	}

	/* Creates a new document, a node and set it as a root node */
	doc = xmlNewDoc(BAD_CAST "1.0");
	if(doc == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_ntp_done;
	}

	root_node = xmlNewNode(NULL, BAD_CAST "alcatel_73xx_ntp");
	if(root_node == NULL)
	{
		status->status = MDM_OP_ERROR;
		sprintf(status->status_message, "Error creating doc xml.");
		goto dslam_alcatel_73xx_get_ntp_done;
	}
	xmlDocSetRootElement(doc, root_node);

	tmp1 = d->exec_buffer;
	for(i = 0; i < 3; i++)
	{
		tmp1 = strstr(tmp1, tokens[i]);
		if(tmp1 == NULL)
			continue;
		tmp1 += tokenslen[i];
		tmp2 = strchr(tmp1, 13);
		snprintf(ntp_buffer, tmp2 - tmp1 + 1, "%s", tmp1);
		/*
		 * Here goes another way to create nodes. xmlNewNode() and xmlNewText
		 * creates a node and a text node separately. They are "attached"
		 * by xmlAddChild() 
		 */
		xmlNewChild(
			root_node, NULL, BAD_CAST tokensnames[i], BAD_CAST ntp_buffer
		);
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
dslam_alcatel_73xx_get_ntp_done:
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
dslam_alcatel_73xx_create_service_profile(
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
dslam_alcatel_73xx_delete_service_profile(
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
dslam_alcatel_73xx_ramodeup_service_profile(
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
dslam_alcatel_73xx_ramodedown_service_profile(
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
dslam_alcatel_73xx_pbitrateup_service_profile(
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
dslam_alcatel_73xx_pbitratedown_service_profile(
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
dslam_alcatel_73xx_mdelayup_service_profile(
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
dslam_alcatel_73xx_mdelaydown_service_profile(
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
dslam_alcatel_73xx_maxbitrateup_service_profile(
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
dslam_alcatel_73xx_maxbitratedown_service_profile(
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
dslam_alcatel_73xx_minbitrateup_service_profile(
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
dslam_alcatel_73xx_minbitratedown_service_profile(
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
dslam_alcatel_73xx_nop(
	mdm_device_descriptor_t *d, mdm_operation_result_t *status
)
{
	d->exec_buffer_post_len = 0;
	return;
}

/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
