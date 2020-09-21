#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <mysql.h>
#include "database.h"


#define STRING_SIZE 500

int update_esb_request(char *status, int id)
{

    printf("Updating esb_request status...\n");

    MYSQL_STMT    *stmt;
    MYSQL_BIND     bind[2];
    my_ulonglong   affected_rows;
    int            param_count;
    int            id_data;
    char           status_data[STRING_SIZE];
    unsigned long  str_length;

    MYSQL *con; /*database connection handle*/
    /**
     * @brief Allocates or initialises a MYSQL object 
     * suitable for mysql_real_connect() function
     * 
     */
    con = mysql_init(NULL);

    /**
     * @brief Prints error message incase
     * initialisation of con fails.
     */
    if (con == NULL)
    {

        fprintf(stderr, "mysql_init() failed\n");
        exit(1);
    }

    /**
     * @brief Checks if connection is 
     * properly established.
     * 
     */
    if (mysql_real_connect(con, SERVER,USER,PASSWORD,DATABASE,PORT,UNIX_SOCKET,FLAG) == NULL)
    {
        fprintf(stderr, "Error [%d]: %s \n",mysql_errno(con),mysql_error(con));
        mysql_close(con);
        return -1;
    }
    /* Prepare an UPDATE query with 3 parameters */
    stmt = mysql_stmt_init(con);
    if (!stmt)
    {
        fprintf(stderr, " mysql_stmt_init(), out of memory\n");
        return -1;
    }

    if (mysql_stmt_prepare(stmt, UPDATE_ESB_REQUEST, strlen(UPDATE_ESB_REQUEST)))
    {
        fprintf(stderr, " mysql_stmt_prepare(), UPDATE failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return -1;
    }
    //fprintf(stdout, "prepare, UPDATE successful\n");

    /* Get the parameter count from the statement */
    param_count = mysql_stmt_param_count(stmt);
    //fprintf(stdout, " total parameters in UPDATE: %d\n", param_count);

    /* validate parameter count */
    if (param_count != 2)
    {
        fprintf(stderr, " invalid parameter count returned by MySQL\n");
        return -1;
    }

    /* Bind the data for 2 parameters */
    memset(bind, 0, sizeof(bind));

    /* STRING PARAM */
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char *)&status_data;
    bind[0].is_null = 0;
    bind[0].length = &str_length;
    bind[0].buffer_length = STRING_SIZE;

    /* INTEGER id PARAM */
    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = (char *)&id_data;
    bind[1].is_null = 0;
    bind[1].length = 0;

    /* Bind the buffers */
    if (mysql_stmt_bind_param(stmt, bind))
    {
        fprintf(stderr, " mysql_stmt_bind_param() failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return -1;
    }

    id_data = id;
    strncpy(status_data, status, STRING_SIZE);
    str_length = strlen(status_data);

    /* Execute the UPDATE statement*/
    if (mysql_stmt_execute(stmt))
    {
        fprintf(stderr, " mysql_stmt_execute, failed\n");
        fprintf(stderr, " [%d]%s\n", mysql_stmt_errno(stmt),
                mysql_stmt_error(stmt));
        return -1;
    }

    /* Get the total rows affected */
    affected_rows = mysql_stmt_affected_rows(stmt);
    fprintf(stdout, "Total affected rows(UPDATE ): %lu\n",
            (unsigned long)affected_rows);

    /* validate affected rows */
    if (affected_rows != 1) {
    fprintf(stderr, " invalid affected rows by MySQL\n");
        return -1;
    }

    /* Close the statement */
    if (mysql_stmt_close(stmt))
    {
        fprintf(stderr, " failed while closing the statement\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return -1;
    }
    //printf("connection id: %ld\n", mysql_thread_id(con));

    mysql_close(con);
    return 1;
}

/*
int main()
{
  int i = update_esb_request("PROCESSING",13);
  printf("%d\n",i);
  return 0;
}
*/