#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define IP "34.118.48.238"
#define PORT 8080
#define CONTENT_TYPE "application/json"
#define PATH_AUTHENTIFICATION "/api/v1/tema/auth/"
#define PATH_LIBRARY "/api/v1/tema/library/"

// realizeaza comanda de register
void register_command(int sockfd) {
    char *message, *response, *username, *password, aux[BUFLEN];

   // initializare obiect json
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    // se citeste username-ul
    printf("username=");
    fgets(aux, BUFLEN - 1, stdin);
    username = strtok(aux, "\n");
            
    // creeaza camp json pentru username
    json_object_set_string(root_object, "username", username);
            
    // se citeste parola
    printf("password=");
    fgets(aux, BUFLEN - 1, stdin);
    password = strtok(aux, "\n");

    // creeaza camp json pentru password
    json_object_set_string(root_object, "password", password);

    char *payload = json_serialize_to_string_pretty(root_value);

    // completam calea 
    char complete_path[50];
    strcpy(complete_path, PATH_AUTHENTIFICATION);
    strcat(complete_path, "register");

    // se creeaza o cerere de tip POST
    message = compute_post_request(IP, complete_path, CONTENT_TYPE, &payload, 1, NULL, 0, NULL);

    // se trimite crerea catre server
    send_to_server(sockfd, message);

    // se primeste rapsunul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " ");

    // se verifica codul rapsunsului de la server
    if (atoi(token) / 100 == 2) {
        printf("[SUCCESS] Register: %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n\n", token);
    } else {
        printf("[ERROR] Can't register: %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);

        printf("%s\n\n", basic_extract_json_response(response));
    }

    json_free_serialized_string(payload);
    json_value_free(root_value);
    free(message);
    free(response);
}

// realizeaza comanda de login
char* login_command(int sockfd) {
 
    int logged = 0;
    char *message, *response, *username, *password, aux[BUFLEN], *wanted_cookie;
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    // se citeste username-ul
    printf("username=");
    fgets(aux, BUFLEN - 1, stdin);
    username = strtok(aux, "\n");
    
    // se creeaza camp json pentru suername       
    json_object_set_string(root_object, "username", username);

    // se citeste parola
    printf("password=");
    fgets(aux, BUFLEN - 1, stdin);
    password = strtok(aux, "\n");

    // creaza camp json pentru parola
    json_object_set_string(root_object, "password", password);

    char *payload = json_serialize_to_string_pretty(root_value);

    // se completeaza calea de acces
    char complete_path[50];
    strcpy(complete_path, PATH_AUTHENTIFICATION);
    strcat(complete_path, "login");

    // se creeaza o cerere de tip POST
    message = compute_post_request(IP, complete_path, CONTENT_TYPE, &payload, 1, NULL, 0, NULL);

    // se trimite cererea catre server
    send_to_server(sockfd, message);

    // se primeste raspunsul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " "); 

    // se verifica daca cererea a fost executat cu succes
    if (atoi(token) / 100 == 2) {

        printf("[SUCCESS] Login: %s ", token);

        token = strtok(NULL, "\n");
        printf("%s\n", token);

        // se preia cookie-ul din raspunsul server-ului
        wanted_cookie = strstr(response, "Set-Cookie: ");
        wanted_cookie = strtok(wanted_cookie, " ");
        wanted_cookie = strtok(NULL, ";");
                
        printf("The Cookie is: %s\n\n", wanted_cookie);
        logged = 1; // utilizatorul s-a logat

    } else {
        printf("[ERROR] Can't login: %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);

        printf("%s\n\n", basic_extract_json_response(response));
    }

    free(message);
    close_connection(sockfd);
    json_free_serialized_string(payload);
    json_value_free(root_value);
    free(response);

    // se verifica daca clientul s-a logat
    if (logged == 1) {
        return wanted_cookie;
    }
    return NULL;
}

void enter_library_command(int sockfd, char **cookie, char **jwt) {

    // se verifica daca clientul este logat
    if (*cookie == NULL) {
        printf("You are not logged in!\n\n");
        return;        
    }

    if (*jwt != NULL) {
        printf("You already have authorization!\n\n");
    }

    char *message, *response;

    // se completeaza calea de acces
    char complete_path[50];
    strcpy(complete_path, PATH_LIBRARY);
    strcat(complete_path, "access");

    // se creeaza o cerere de tip GET
    message = compute_get_request(IP, complete_path, NULL, *cookie, 1, NULL);

    // se trimite cererea catre server
    send_to_server(sockfd, message);

    // se preia raspunsul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " "); 

    // se verifica daca cererea s-a realizat cu succes
    if (atoi(token) / 100 == 2) {
        printf("[SUCCESS] You got authorization to the library: %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);

        // se extrage token-ul JWT din raspunsul server-ului
        token = basic_extract_json_response(response); 
        token = strstr(token, ":");
        (*jwt) = strdup(token + 2);
        *(*jwt + strlen(*jwt) - 2) = 0;
        printf("JWT Token is: %s\n\n", *jwt);
    } else {
        printf("[ERROR] Can't enter library: %s ", token);
        token = strtok(NULL, " ");
        printf("%s\n\n", token);
    }

    free(message);
    free(response);
}

// realizeaza comanda GET BOOKS
void get_books_command(int sockfd, char **cookie, char **jwt) {
    // se verifica daca clientul este logat
    if (*cookie == NULL) {
        printf("You are not logged in!\n\n");
        return;
    } 

    // se verifica daca clientul are autorizatie
    if (*jwt == NULL) {
        printf("You don't have authorization to library!\n\n");
        return;
    }

    // se completeaza calea de acces
    char *message, *response;
    char complete_path[50];
    strcpy(complete_path, PATH_LIBRARY);
    strcat(complete_path, "books");

    // se creeaza o cerere de tip GET
    message = compute_get_request(IP, complete_path, NULL, *cookie, 1, *jwt);

    // se trimite cererea catre server
    send_to_server(sockfd, message);

    // se primeste raspunsul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " "); 

    // se verifica daca cererea s-a realizat cu succes
    if (atoi(token) / 100 == 2) {
        printf("[SUCCESS] The books in your library are: %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);
        
        // se verifica daca clientul are carti in biblioteca sa        
        if (basic_extract_json_response(response) == NULL) {
            printf("[]\n\n");
        } else {
            printf("[%s\n\n", basic_extract_json_response(response));
        }

    } else {
        
        printf("[ERROR] Can't get books %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);
        printf("%s\n", basic_extract_json_response(response));
    }

    free(message);
    free(response);
}

// realizeaza comanda de add book
void add_book_command(int sockfd, char **cookie, char **jwt) {
    // se verifica daca clientul este logat
    if (*cookie == NULL) {
        printf("You are not logged in!\n\n");
        return;
    }
    
    // se verifica daca clientul are autorizatie        
    if (*jwt == NULL) {
        printf("You don't have authorization to the library!\n\n");
        return;
    }

    char *message, *response, *title, *author, *genre, *page_count, *publisher, aux[BUFLEN];
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    // se citeste titlul
    printf("title=");
    memset(aux, 0, BUFLEN);
    fgets(aux, BUFLEN - 1, stdin);
    title = strtok(aux, "\n");
    json_object_set_string(root_object, "title", title);

    // se citeste autorul
    printf("author=");
    memset(aux, 0, BUFLEN);
    fgets(aux, BUFLEN - 1, stdin);
    author = strtok(aux, "\n");
    json_object_set_string(root_object, "author", author);

    // se citeste genul cartii
    printf("genre=");
    memset(aux, 0, BUFLEN);
    fgets(aux, BUFLEN - 1, stdin);
    genre = strtok(aux, "\n");
    json_object_set_string(root_object, "genre", genre);

    // se citeste editorul cartii
    printf("publisher=");
    memset(aux, 0, BUFLEN);
    fgets(aux, BUFLEN - 1, stdin);
    publisher = strtok(aux, "\n");
    json_object_set_string(root_object, "publisher", publisher);

    // se citesc numarul de pagini pe care o are cartea
    printf("page_count=");
    memset(aux, 0, BUFLEN);
    fgets(aux, BUFLEN - 1, stdin);
    page_count = strtok(aux, "\n");
    json_object_set_string(root_object, "page_count", page_count);

    char *payload = json_serialize_to_string_pretty(root_value);

    // se completeaza calea de acces
    char complete_path[50];
    strcpy(complete_path, PATH_LIBRARY);
    strcat(complete_path, "books");

    // se creeaza o cerere de tip POST
    message = compute_post_request(IP, complete_path, CONTENT_TYPE, &payload, 1, NULL, 0, *jwt);

    // se trimite cerea catre server
    send_to_server(sockfd, message);

    // se primeste raspunsul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " "); 

    // se verifica daca cererea s-a realizat cu succes
    if (atoi(token) / 100 == 2) {
        printf("[SUCCESS] The book is added: %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n\n", token);
    } else {
        printf("[ERROR] Can't add book: %s ", token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);
        printf("%s\n\n", basic_extract_json_response(response));
    }
    
    json_free_serialized_string(payload);
    json_value_free(root_value);
    free(message);
    free(response);
}

// realizeaza comanda de get book
void get_book_command(int sockfd, char **cookie, char **jwt) {
    // se verifica daca clientul este logat
    if (*cookie == NULL) {
        printf("You are not logged in!\n\n");
    }

    // se verifica daca clientul are autorizatie
    if (*jwt == NULL) {
        printf("You don't have authorization!\n\n");
        return;
    }

    char *message, *response, *id, aux[BUFLEN];

    // se citeste ud-ul cartii
    printf("id=");
    fgets(aux, BUFLEN - 1, stdin);
    id = strtok(aux, "\n");
    
    // se completeaza calea de acces
    char complete_path[50];
    strcpy(complete_path, PATH_LIBRARY);
    strcat(complete_path, "books/");
    strcat(complete_path, id);

    // se creeaza o cerere de tip GET
    message = compute_get_request(IP, complete_path, NULL, *cookie, 1, *jwt);

    // se trimite cererea catre server
    send_to_server(sockfd, message);

    // se primeste raspunsul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " "); 

    // se verifica daca cererea s-a realizat cu succes
    if (atoi(token) / 100 == 2) {
        printf("[SUCCESS] Got the book with ID = %s: %s ", id, token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);
        printf("%s\n\n", basic_extract_json_response(response));
    } else {
        printf("[ERROR] Can't get book with ID = %s : %s ", id, token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);
        printf("%s\n\n", basic_extract_json_response(response));
    }

    free(message);
    free(response);
}

// realizeaza comanda de delete book
void delete_book_command(int sockfd, char **cookie, char **jwt) {
    // se verifica daca clientul este logat
    if (*cookie == NULL) {
        printf("Youe are not logged in!\n\n");
        return;
    }

    // se verifica daca clientul are autorizatie
    if (*jwt == NULL) {
        printf("You don't have authorization to the library\n\n");
        return;
    }

    char *message, *response, *id, aux[BUFLEN];
    
    // se citeste id-ul cartii        
    printf("id=");
    fgets(aux, BUFLEN - 1, stdin);
    id = strtok(aux, "\n");
    
    // se completeaza calea de acces       
    char complete_path[50];
    strcpy(complete_path, PATH_LIBRARY);
    strcat(complete_path, "books/");
    strcat(complete_path, id);

    // se creeaza o cerere de tip DELETE
    message = compute_delete_request(IP, complete_path, *cookie, 1, *jwt);

    // se trimite cererea catre server
    send_to_server(sockfd, message);

    // se primeste raspunsul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " "); 

    // se verifica daca cererea s-a realizat cu succes
    if (atoi(token) / 100 == 2) {
        printf("[SUCCESS] The book with ID = %s is deleted: %s ", id, token);
        token = strtok(NULL, "\n");
        printf("%s\n\n", token);
    } else {
        printf("[ERROR] Can't delete book with ID = %s : %s ", id, token);
        token = strtok(NULL, "\n");
        printf("%s\n", token);
        printf("%s\n\n", basic_extract_json_response(response));
    }

    free(message);
    free(response);
}

// realizeaza comanda de logout
void logout_command(int sockfd, char *cookie) {
    // verifica daca clientul este logat
    if (cookie == NULL) {
        printf("You are not logged in!\n\n");
        return;
    }

    // se completeaza calea catre acces
    char *response, *message;
    char complete_path[50];
    strcpy(complete_path, PATH_AUTHENTIFICATION);
    strcat(complete_path, "logout");

    // se creeaza o cerere de tip GET
    message = compute_get_request(IP, complete_path, NULL, cookie, 1, NULL);

    // se trimite cererea catre server
    send_to_server(sockfd, message);

    // se preia raspunsul de la server
    response = receive_from_server(sockfd);

    char copy_response[strlen(response)];
    strcpy(copy_response, response);
    char *token = strtok(copy_response, " ");
    token = strtok(NULL, " "); 

    // se verifica dac cerea s-a realizat cu succes
    if (atoi(token) / 100 == 2) {
                
    printf("[SUCCESS] You are logged out: %s ", token);
    token = strtok(NULL, "\n");
    printf("%s\n\n", token);
            
    } else {
        printf("[ERROR] Can't logout: %s ", token);
        token = strtok(NULL, " ");
        printf("%s\n", token);
        printf("%s\n", basic_extract_json_response(response));
    }

    free(message);
    free(response);
}

int main(int argc, char *argv[])
{
    int sockfd;
    char command[50];
    char *cookie, *jwt;
    cookie = NULL;
    jwt = NULL;
        
    while (1) {

        // se citeste comanda de la stdin
        scanf("%s", command);
        getchar();
        
        // se deschide socket-ul
        sockfd = open_connection(IP, PORT, AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
    	   fprintf(stderr, "Can't open socket\n");
    	   exit(0);
        }

        if (strcmp(command, "exit") == 0) {
            close_connection(sockfd);
            break;
        } 

        if (strcmp(command, "register") == 0) {
            register_command(sockfd);
            close_connection(sockfd);
            continue;
        } 

        if (strcmp(command, "login") == 0) {
            // se verifica daca un client s-a logat deja
            if (cookie != NULL) {
                printf("Already logged in!\n\n");
                continue;  
            }
            cookie = login_command(sockfd);
            close_connection(sockfd);
            continue;
        } 

        if (strcmp(command, "enter_library") == 0) {
            enter_library_command(sockfd, &cookie, &jwt);
            close_connection(sockfd);           
            continue;
            
        } 

        if (strcmp(command, "get_books") == 0) {
            get_books_command(sockfd, &cookie, &jwt);
            close_connection(sockfd);
            continue;
        }

        if (strcmp(command, "get_book") == 0) {

            get_book_command(sockfd, &cookie, &jwt);
            close_connection(sockfd);
            continue;

        } 
        
        if (strcmp(command, "add_book") == 0) {
            add_book_command(sockfd, &cookie, &jwt);
            close_connection(sockfd);
            continue;
        } 

        if (strcmp(command, "delete_book") == 0) {
            delete_book_command(sockfd, &cookie, &jwt);
            close_connection(sockfd);
            continue;
        } 

        if (strcmp(command, "logout") == 0) {

            logout_command(sockfd, cookie);

            close_connection(sockfd);
            cookie = NULL;

            if (jwt != NULL) {
                free(jwt);
                jwt = NULL;
            }
            
            continue;
        } 
        
        printf("Wrong Command!\n\n");

        close_connection(sockfd);

    }

    if (jwt != NULL) {
        free(jwt);
    }

    return 0;
}
