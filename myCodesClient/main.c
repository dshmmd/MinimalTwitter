#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"

//client
int initialize();
char *send_data (char *);
struct sockaddr_in server;

//global variables
int port = 12345;
char token[100];

//functions
void showRegisterMenu();
void signUp();
int logIn();
void showMainMenu();
void showTimeLineMenu();
void sendTweet();
void refresh();
void showLikeCmMenu();
void like();
void comment();
void searchMenu();
void showLikeCmMenuInProfile(char username[100]);
void searchByUserName(char userName[100]);
void follow(char username[100]);
void unfollow(char username[100]);
void tweetProfile();
void showPersonalAreaMenu();
void setBio();
void changePassword();
void logout();
int isThereSpace(char * input);

int main() {
    int can_initialize = initialize();
    if (can_initialize == 0 ) {
        showRegisterMenu();
    }
    return 0;
}

int initialize() {
    WSADATA wsadata;
    int wsaStartUp = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (wsaStartUp != 0) {
        printf("Error");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.8.8.1");
    server.sin_port = htons(port);
    return 0;
}

char *send_data (char *data) {
    char *buffer=malloc(1000);
    memset(buffer, 0, 1000);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        return buffer;
    }

    int can_connect = connect(client_socket, (struct sockaddr *) &server, sizeof(server));
    if (can_connect != 0) {
        return buffer;
    }

    send(client_socket, data, strlen(data), 0);
    recv(client_socket, buffer, 999, 0);
    closesocket(client_socket);
    return buffer;
}

void showRegisterMenu() {
    int choice;

    do {
        printf("Register menu!\nEnter a number!\n1.Signup\n2.Login\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>2 || choice<1);

    if (choice == 1) {
        signUp();
        showRegisterMenu();
    } else {
        if (logIn() == 1) {
            showMainMenu();
        } else {
            showRegisterMenu();
        }
    }
}

void signUp() {
    printf("Username:\n");
    char userName[100];
    gets(userName);

    printf("Password:\n(Do not use the space character)\n");
    char passWord[100];
    gets(passWord);

    printf("Re-enter your password:\n");
    char passWord2[100];
    gets(passWord2);

    if (strcmp(passWord, passWord2) == 0 && isThereSpace(passWord) == 0) {
        char data[210];
        sprintf(data, "signup %s %s\n", userName, passWord);

        char * result = send_data(data);

        cJSON *root = cJSON_Parse(result);
        cJSON *type = cJSON_GetObjectItem(root, "type");
        cJSON *message = cJSON_GetObjectItem(root, "message");

        if (strcmp(type->valuestring, "Successful") == 0) {
            system("cls");
            printf("THANK YOU!!!\nYour account was created successfully!!!\n");
        } else {
            system("cls");
            printf("SORRY!\n%s\n", message->valuestring);
        }
    } else {
        system("cls");
        printf(isThereSpace(passWord) ? "Do not use the space character.\n" : "The second password is invalid!!!\n");
        signUp();
    }
}

int logIn() {
    printf("Username:\n");
    char userName[100];
    gets(userName);

    printf("Password:\n");
    char passWord[100];
    gets(passWord);

    char data[210];
    sprintf(data, "login %s %s\n", userName, passWord);
    char * result = send_data(data);

    cJSON *jsObject = cJSON_Parse(result);
    cJSON *type = cJSON_GetObjectItem(jsObject, "type");
    cJSON *message = cJSON_GetObjectItem(jsObject, "message");

    if (strcmp(type->valuestring, "Token") == 0) {
        strcpy(token, message->valuestring);
        system("cls");
        printf("WELCOME!!!\nYou have successfully logged in to your account!\n");
        return 1;
    } else {
        system("cls");
        printf("SORRY!\n%s\n", message->valuestring);
    }
}

void showMainMenu() {
    int choice;

    do {
        printf("main menu!\nEnter a number:\n1.Timeline\n2.Search\n3.Tweet Profile\n4.Personal area\n5.Log out\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>5 || choice<1);

    if (choice == 1) {
        showTimeLineMenu();
    } else if (choice == 2) {
        searchMenu();
    } else if (choice == 3) {
        tweetProfile();
    } else if (choice == 4) {
        showPersonalAreaMenu();
    } else {
        logout();
    }
}

void showTimeLineMenu() {
    int choice;

    do {
        printf("Time line menu!\nEnter a number:\n1.Send tweet\n2.Refresh\n3.Back\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>3 || choice<1);

    if (choice == 1) {
        sendTweet();
    } else if (choice == 2){
        refresh();
    } else {
        showMainMenu();
    }
}

void sendTweet() {
    printf("type your new tweet:\n");

    char tweetText[300];
    gets(tweetText);

    char data[415];
    sprintf(data, "sendTweet %s %s\n", token, tweetText);

    char * result = send_data(data);

    cJSON *root = cJSON_Parse(result);
    cJSON *message = cJSON_GetObjectItem(root, "message");

    system("cls");
    printf("%s\n", message->valuestring);
    showTimeLineMenu();
}

void refresh() {
    char data[110];
    sprintf(data, "refresh %s\n", token);
    char * result = send_data(data);

    cJSON *root = cJSON_Parse(result);
    cJSON *message = cJSON_GetObjectItem(root,"message");

    int arrayLength = cJSON_GetArraySize(message);
    for (int i=0; i<arrayLength; i++) {
        cJSON *thisObject = cJSON_GetArrayItem(message, i);

        cJSON *id = cJSON_GetObjectItem(thisObject, "id");
        cJSON *author = cJSON_GetObjectItem(thisObject, "author");
        cJSON *content = cJSON_GetObjectItem(thisObject, "content");
        cJSON *likes = cJSON_GetObjectItem(thisObject, "likes");
        cJSON *comments = cJSON_GetObjectItem(thisObject, "comments");

        cJSON *obj = NULL;
        int cmCount = 0;
        cJSON_ArrayForEach(obj, comments) {
            cmCount++;
        }

        printf("Tweet by: %s\n%s\n\n", author->valuestring, content->valuestring);
        printf("Likes: %d\tComments: %d\tTweet Id: %d\n\n\n", likes->valueint, cmCount, id->valueint);
    }

    int choice;

    do {
        printf("###\n1.Like or comment a tweet!\n2.Unfollow someone...\n3.Back\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>3 || choice<1);

    if (choice==1) {
        showLikeCmMenu();
    } else if (choice==2) {
        printf("could you tell me the username you want to unfollow?\n");
        char username[100];
        gets(username);
        unfollow(username);
        refresh();
    } else {
        showTimeLineMenu();
    }
}

void showLikeCmMenu() {
    int choice;

    do {
        printf("1.Like\n2.Comment\n3.Back\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>3 || choice<1);

    if (choice == 1) {
        like();
    } else if (choice == 2) {
        comment();
    } else {
        refresh();
    }
}

void like() {
    printf("tweet ID you want to like?\n");
    char tweetId[50];
    gets(tweetId);

    char data[160];
    sprintf(data, "like %s %s\n", token, tweetId);

    char * result = send_data(data);

    cJSON *root = cJSON_Parse(result);
    cJSON *message = cJSON_GetObjectItem(root, "message");

    system("cls");
    printf("%s\n", message->valuestring);
    showLikeCmMenu();
}

void comment() {
    printf("tweet ID you want to comment on?\n");
    char tweetId[50];
    gets(tweetId);
    system("cls");

    printf("Type your comment!\n");
    char comment[100];
    gets(comment);

    char data[260];
    sprintf(data, "comment %s %s %s\n", token, tweetId, comment);

    char * result = send_data(data);

    cJSON *root = cJSON_Parse(result);
    cJSON *message = cJSON_GetObjectItem(root, "message");

    system("cls");
    printf("%s\n", message->valuestring);
    showLikeCmMenu();
}

void searchMenu() {
    printf("Enter the username you want to find?\n(ENTER 0 TO BACK)\n");
    char userName[100];
    gets(userName);
    system("cls");

    if (strcmp(userName, "0") == 0) {
        showMainMenu();
    } else {
        searchByUserName(userName);
    }
}

void searchByUserName(char * userName) {
    char data[210];
    sprintf(data, "search %s %s\n", token, userName);

    char * result = send_data(data);

    cJSON *root = cJSON_Parse(result);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root,"message");

    if (strcmp(type->valuestring, "Profile") == 0) {
        cJSON *user = cJSON_GetObjectItem(message, "username");
        cJSON *bio = cJSON_GetObjectItem(message, "bio");
        cJSON *numberOfFollowers = cJSON_GetObjectItem(message, "numberOfFollowers");
        cJSON *numberOfFollowings = cJSON_GetObjectItem(message, "numberOfFollowings");
        cJSON *followStatus = cJSON_GetObjectItem(message, "followStatus");
        cJSON *allTweets = cJSON_GetObjectItem(message, "allTweets");

        int status = (strcmp(followStatus->valuestring, "NotFollowed") == 0 ? 0 : 1);

        int choice;

        do {
            printf("username: %s\n", user->valuestring);
            printf("bio: %s\n", bio->valuestring);
            printf("Followers: %d   Followings: %d\n", numberOfFollowers->valueint, numberOfFollowings->valueint);
            printf("Follow status: %s\n", followStatus->valuestring);

            printf("\n%s's tweets:\n", userName);

            int arrayLength = cJSON_GetArraySize(allTweets);
            for (int i=0; i<arrayLength; i++) {
                cJSON *thisObject = cJSON_GetArrayItem(allTweets, i);

                cJSON *id = cJSON_GetObjectItem(thisObject, "id");
                cJSON *content = cJSON_GetObjectItem(thisObject, "content");
                cJSON *likes = cJSON_GetObjectItem(thisObject, "likes");
                cJSON *comments = cJSON_GetObjectItem(thisObject, "comments");

                cJSON *obj = NULL;
                int cmCount = 0;

                cJSON_ArrayForEach(obj, comments) {
                    cmCount++;
                }

                printf("\n%s\n", content->valuestring);
                printf("Likes: %d\tComments: %d\tTweet ID: %d\n", likes->valueint, cmCount, id->valueint);
            }

            if (status) {
                printf("###\n1.Unfollow this user\n2.Like or comment a tweet!\n3.Back\n");
            } else {
                printf("###\n1.Follow this user\n2.Like or comment a tweet!\n3.Back\n");
            }
            scanf("%d", &choice);
            fflush(stdin);
            system("cls");
        } while (choice>3 || choice<1);

        if (choice == 1) {
            if (status) {
                system("cls");
                unfollow(userName);
                searchByUserName(userName);
            } else {
                system("cls");
                follow(userName);
                searchByUserName(userName);
            }
        } else if (choice == 2) {
            system("cls");
            showLikeCmMenuInProfile(userName);
        } else {
            system("cls");
            searchMenu();
        }
    } else {
        system("cls");
        printf("%s\n", message->valuestring);
        searchMenu();
    }
}

void showLikeCmMenuInProfile(char username[100]) {
    printf("Tweet ID?\n");
    char tweetId[50];
    gets(tweetId);

    int choice;
    system("cls");
    do {
        printf("1.Like\n2.Comment\n3.Back\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>3 || choice<1);

    if (choice == 1) {
        char data[160];
        sprintf(data, "like %s %s\n", token, tweetId);

        char * result = send_data(data);

        cJSON *root = cJSON_Parse(result);
        cJSON *message = cJSON_GetObjectItem(root, "message");

        system("cls");
        printf("%s\n", message->valuestring);
        searchByUserName(username);
    } else if (choice == 2) {
        printf("Type your comment!\n");
        char comment[100];
        gets(comment);

        char data[260];
        sprintf(data, "comment %s %s %s\n", token, tweetId, comment);

        char * result = send_data(data);

        cJSON *root = cJSON_Parse(result);
        cJSON *message = cJSON_GetObjectItem(root, "message");

        system("cls");
        printf("%s\n", message->valuestring);
        searchByUserName(username);
    } else {
        system("cls");
        searchByUserName(username);
    }
}

void follow(char username[100]) {
    char data[210];
    sprintf(data, "follow %s %s\n", token, username);
    char *result = send_data(data);
    cJSON *root = cJSON_Parse(result);
    cJSON *message = cJSON_GetObjectItem(root, "message");

    system("cls");
    printf("%s\n", message->valuestring);
}

void unfollow(char username[100]) {
    char data[210];
    sprintf(data, "unfollow %s %s\n", token, username);
    char *result = send_data(data);
    cJSON *root = cJSON_Parse(result);
    cJSON *message = cJSON_GetObjectItem(root, "message");

    system("cls");
    printf("%s\n", message->valuestring);
}

void tweetProfile() {
    char data[110];
    sprintf(data, "profile %s\n", token);

    char * result = send_data(data);

    cJSON *root = cJSON_Parse(result);
    cJSON *type = cJSON_GetObjectItem(root, "type");
    cJSON *message = cJSON_GetObjectItem(root,"message");

    if (strcmp(type->valuestring, "Profile") == 0) {
        cJSON *user = cJSON_GetObjectItem(message, "username");
        cJSON *bio = cJSON_GetObjectItem(message, "bio");
        cJSON *numberOfFollowers = cJSON_GetObjectItem(message, "numberOfFollowers");
        cJSON *numberOfFollowings = cJSON_GetObjectItem(message, "numberOfFollowings");
        cJSON *allTweets = cJSON_GetObjectItem(message, "allTweets");

        int choice;

        do {
            printf("username: %s\n", user->valuestring);
            printf("bio: %s\n", bio->valuestring);
            printf("Followers: %d   Followings: %d\n", numberOfFollowers->valueint, numberOfFollowings->valueint);

            printf("\nYour tweets:\n");

            int arrayLength = cJSON_GetArraySize(allTweets);
            for (int i=0; i<arrayLength; i++) {
                cJSON *thisObject = cJSON_GetArrayItem(allTweets, i);

                cJSON *id = cJSON_GetObjectItem(thisObject, "id");
                cJSON *content = cJSON_GetObjectItem(thisObject, "content");
                cJSON *likes = cJSON_GetObjectItem(thisObject, "likes");
                cJSON *comments = cJSON_GetObjectItem(thisObject, "comments");

                cJSON *obj = NULL;
                int cmCount = 0;

                cJSON_ArrayForEach(obj, comments) {
                    cmCount++;
                }

                printf("\n%s\n", content->valuestring);
                printf("Likes: %d\tComments: %d\tTweet ID: %d\n", likes->valueint, cmCount, id->valueint);
            }

            printf("###\n1.Back\n");
            scanf("%d", &choice);
            fflush(stdin);
            system("cls");
        } while (choice != 1);

        system("cls");
        showMainMenu();

    } else {
        system("cls");
        printf("%s\n", message->valuestring);
        showMainMenu();
    }
}

void showPersonalAreaMenu() {
    int choice;

    do {
        printf("Personal area!\nEnter a number!\n1.Set Bio\n2.Change Password\n3.Back\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>3 || choice<1);

    if (choice == 1) {
        setBio();
    } else if (choice == 2){
        changePassword();
    } else {
        showMainMenu();
    }
}

void setBio() {
//    int choice;

//    do {
//        printf("You can only set your bio once!\n1.Continue\n2.Back\n");
//        scanf("%d", &choice);
//        fflush(stdin);
//        system("cls");
//    } while (choice>2 || choice<1);

//    if (choice == 1) {
        char bio[150];
        printf("Enter your bio:\n");
        gets(bio);

        char data[260];
        sprintf(data, "setBio %s %s\n", token, bio);

        char * result = send_data(data);

        cJSON *root = cJSON_Parse(result);
        cJSON *message = cJSON_GetObjectItem(root, "message");

        system("cls");
        printf("%s\n", message->valuestring);
        showPersonalAreaMenu();
//    } else {
//        showPersonalAreaMenu();
//    }
}

void changePassword() {
    printf("Enter your current password:\n");
    char oldPassword[100];
    gets(oldPassword);

    printf("Enter your new password:\n");
    char passWord[100];
    gets(passWord);

    printf("Re-enter your new password:\n");
    char passWord2[100];
    gets(passWord2);

    if (strcmp(passWord, passWord2) == 0 && isThereSpace(passWord) == 0) {
        char data[320];
        sprintf(data, "changePassword %s %s %s\n", token, oldPassword, passWord);

        char * result = send_data(data);

        cJSON *root = cJSON_Parse(result);
        cJSON *message = cJSON_GetObjectItem(root, "message");

        system("cls");
        printf("%s\n", message->valuestring);
        showPersonalAreaMenu();
    } else {
        system("cls");
        printf(isThereSpace(passWord) ? "Do not use the space character.\n" : "The second password is invalid!!!\n");
        showPersonalAreaMenu();
    }
}

void logout() {
    int choice;

    do {
        printf("Are you sure?\n1.YES!\n2.NO!\n");
        scanf("%d", &choice);
        fflush(stdin);
        system("cls");
    } while (choice>2 || choice<1);

    if (choice == 1) {
        char data[110];
        sprintf(data, "logout %s\n", token);
        send_data(data);

        system("cls");
        printf("SEE YOU LATER!\n");
        showRegisterMenu();
    } else {
        system("cls");
        showMainMenu();
    }
}

int isThereSpace(char * input) {
    int length = strlen(input);
    int spacesExist = 0;

    for (int i = 0; i<length; i++) {
        if(input[i] == ' ') {
            spacesExist = 1;
            return spacesExist;
        }
    }

    return spacesExist;
}