#ifndef _REQUESTS_
#define _REQUESTS_

// creeaza si returneaza un string pentru cererea de tip GET
char *compute_get_request(char *host, char *url, char *query_params,
							char *cookies, int cookies_count, char *jwt);

// creeaza si returneaza un string pentru cererea de tip POST
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char* cookies, int cookies_count, char *jwt);

// creeaza si returneaza un string pentru cererea de tip DELETE
char *compute_delete_request(char *host, char *url, char *cookies, int cookies_count, char *jwt);

#endif
