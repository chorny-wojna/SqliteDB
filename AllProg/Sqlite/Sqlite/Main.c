/*
	sqlite commands
*/


#include <stdio.h>
#include <locale.h>
#include "sqlite3.h"
#include <stdbool.h>


static int callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	fprintf(stderr, "%s: ", (const char*)data);
	for (i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "russian");

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[512];
	const char* data = "Callback function called";
	int max_id = 10;

	rc = sqlite3_open("companystruct.db", &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}

	while (true)
	{
		printf("Main Menu\n");
		printf("1) Select data\n");
		printf("2) Insert data\n");
		printf("3) Delete data\n");
		printf("4) Upload employee photo\n");
		printf("Any other character to exit\n");

		char c = getch();

		switch (c)
		{
		case '1':
		{
			printf("Parameters:\n");
			printf("1) all\n");
			printf("2) by id\n");
			printf("3) by last name\n");
			printf("4) by country\n");
			char a = getch();
			
			strcpy(sql, "SELECT * from employees ");
			switch (a)
			{
			case '1':
			{		
				break;
			}
			case '2':
			{
				printf("id = ");
				int d;
				scanf("%d", &d);
				printf("\n");
				strcat(sql, "where employee_id = ");
				char buf [256];
				sprintf(buf, "%d", d);
				strcat(sql, buf);

				char s[50] = "image";
				strcat(s, buf);
				strcat(s, ".jpg");				

				rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
				if (rc != SQLITE_OK) {
					fprintf(stderr, "SQL error: %s\n", zErrMsg);
					sqlite3_free(zErrMsg);
				}
				else {
					fprintf(stdout, "Operation done successfully\n");
				}

				strcpy(sql, "SELECT image from employees where employee_id = ");
				strcat(sql, buf);

				sqlite3_stmt *pStmt;
				rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);

				if (rc != SQLITE_OK) {
					fprintf(stderr, "Failed to prepare statement\n");
					fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
					sqlite3_close(db);
					return 1;
				}

				rc = sqlite3_step(pStmt);

				int bytes = 0;

				if (rc == SQLITE_ROW) {
					bytes = sqlite3_column_bytes(pStmt, 0);
				}

				FILE* file = fopen(s, "wb");

				fwrite(sqlite3_column_blob(pStmt, 0), bytes, 1, file);

				fclose(file);
				continue;
			}
			case '3':
			{
				printf("last name = ");
				char s[256];
				scanf("%255s", s);
				printf("\n");
				strcat(sql, "where last_name = '");				
				strcat(sql, s);
				strcat(sql, "'");
				break;
			}
			case '4':
			{
				printf("country = ");
				char s[256];
				scanf("%255s", s);
				printf("\n");
				strcat(sql, "where birth_country = '");
				strcat(sql, s);
				strcat(sql, "'");
				break;
			}
			default:
				printf("Nothing was chosen\n");
				return 1;
				break;
			}
			break;
		}
		case '2':
		{
			char first_name[256], last_name[256], middle_name[256],
				birth_date[256], birth_city[256], birth_country[256],
				address[256], department[256], position[256], acceptance_date[256];

			printf("First name: ");
			scanf("%255s", first_name);
			printf("Middle name: ");
			scanf("%255s", middle_name);
			printf("Last name: ");
			scanf("%255s", last_name);
			printf("Birth date: ");
			scanf("%255s", birth_date);
			printf("Birth city: ");
			scanf("%255s", birth_city);
			printf("Birth country: ");
			scanf("%255s", birth_country);
			printf("Address: ");
			scanf("%255s", address);
			printf("Department: ");
			scanf("%255s", department);
			printf("Position: ");
			scanf("%255s", position);
			printf("Acceptance date: ");
			scanf("%255s", acceptance_date);

			strcpy(sql, "INSERT INTO EMPLOYEES VALUES (");

			char buf[50];
			sprintf(buf, "%d", ++max_id);
			strcat(sql, buf);
			strcat(sql, ", '");
			strcat(sql, first_name);
			strcat(sql, "', '");
			strcat(sql, last_name);
			strcat(sql, "', '");
			strcat(sql, middle_name);
			strcat(sql, "', '");
			strcat(sql, birth_date);
			strcat(sql, "', '");
			strcat(sql, birth_city);
			strcat(sql, "', '");
			strcat(sql, birth_country);
			strcat(sql, "', '");
			strcat(sql, address);
			strcat(sql, "', '");
			strcat(sql, department);
			strcat(sql, "', '");
			strcat(sql, position);
			strcat(sql, "', '");
			strcat(sql, acceptance_date);
			strcat(sql, "', NULL); ");

			break;
		}
		case '3':
		{
			char s[50];
			printf("id = ");
			scanf("%49s", s);
			strcpy(sql, "delete from employees where employee_id = ");
			strcat(sql, s);
			break;
		}
		case '4':
		{
			printf("File name = ");
			char filename[256];
			scanf("%s", filename);

			printf("Employee id = ");
			char id[20];
			scanf("%19s", id);

			FILE *fp = fopen(filename, "rb");

			if (fp == NULL) 
			{
				fprintf(stderr, "Cannot open image file\n");
				return 1;
			}

			fseek(fp, 0, SEEK_END);

			if (ferror(fp)) {
				fprintf(stderr, "fseek() failed\n");
				int r = fclose(fp);
				if (r == EOF) {
					fprintf(stderr, "Cannot close file handler\n");
				}

				return 1;
			}

			int flen = ftell(fp);

			if (flen == -1) {

				perror("error occurred");
				int r = fclose(fp);

				if (r == EOF) {
					fprintf(stderr, "Cannot close file handler\n");
				}

				return 1;
			}

			fseek(fp, 0, SEEK_SET);

			if (ferror(fp)) {

				fprintf(stderr, "fseek() failed\n");
				int r = fclose(fp);

				if (r == EOF) {
					fprintf(stderr, "Cannot close file handler\n");
				}

				return 1;
			}

			char* data = malloc(sizeof(char)*(flen + 1));

			int size = fread(data, 1, flen, fp);

			if (ferror(fp)) {

				fprintf(stderr, "fread() failed\n");
				int r = fclose(fp);

				if (r == EOF) {
					fprintf(stderr, "Cannot close file handler\n");
				}

				return 1;
			}

			int r = fclose(fp);

			if (r == EOF) {
				fprintf(stderr, "Cannot close file handler\n");
			}

			char *err_msg = 0;

			sqlite3_stmt *pStmt;

			char rsql[512];
			strcpy(rsql, "update employees set image = ? where employee_id = ");
			strcat(rsql, id);

			rc = sqlite3_prepare(db, rsql, -1, &pStmt, 0);

			if (rc != SQLITE_OK) {

				fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));

				return 1;
			}

			sqlite3_bind_blob(pStmt, 1, data, size, SQLITE_STATIC);

			rc = sqlite3_step(pStmt);

			if (rc != SQLITE_DONE) {

				printf("execution failed: %s", sqlite3_errmsg(db));
			}

			sqlite3_finalize(pStmt);
			
			break;
		}
		default:
			sqlite3_close(db);
			return 1;
			break;
		}

		rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		else {
			fprintf(stdout, "Operation done successfully\n");
		}
	}	

	sqlite3_close(db);
	return 0;
}
