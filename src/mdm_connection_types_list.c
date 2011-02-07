/*!
 * \file mdm_connection_types_list.c Lists all supported connection types.
 *
 * \author Marcelo Gornstein <marcelog@gmail.com>
 */
/*******************************************************************************
 * Include Files.
 ******************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include	<mdm.h>

/*******************************************************************************
 * Forward Declarations.
 ******************************************************************************/
int main(int argc, char *argv[]);

/*******************************************************************************
 * CODE STARTS.
 ******************************************************************************/

/*!
 * Main entry point.
 */
int
main(int argc, char *argv[])
{
	int i = 0;
	const char *connection_name;
	while((connection_name = mdm_connection_2string(i)) != NULL)
	{
		fprintf(stdout, "(%.2d) - %s\n", i, connection_name);
		i++;
	}
	exit(EXIT_SUCCESS);
}
/*******************************************************************************
 * CODE ENDS.
 ******************************************************************************/
