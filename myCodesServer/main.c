#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <time.h>
#include <dirent.h>
#include <ctype.h>
#include "cJSON.h"

#define SIZE 1000
#define PORT 12345

//global variables
char globalToken[32];

//network functions
int initialize(int port);
void start(int server_socket);

//server functions
char * startGame(char *input);
char * signup(char *input);
char * login(char *input);
char * sendTweet(char *input);
char * refresh(char *input);
char * like(char *input);
char * comment(char *input);
char * search(char *input);
char * follow(char *input);
char * unfollow(char *input);
char * profile(char *input);
char * setBio(char *input);
char * changePassword(char *input);
char * logout(char *input);
char * generateError(char * errorText);
char * generateSuccessful(char * messageText);

char * tokenToUsername(char * token);
char * getFileContent(char * fileAddress);
char * giveMeProfile(char * username);
char * giveMeTweet(int id);

int checkBadRequest(char * input, int legalSpaces);
int isThisUserThere(char * fileAddress);
int isThisUserLoggedIn(char * username);
int getTweetCount(void);
int didYouSeeThat(char * username, int tweetId);
int didYouLikeIt(char * username, int tweetId);
int numOfFollowers(char * username);
int numOfFollowings(char * username);
int didYouFollowHim(char * fael, char * mafool);
int findIndexInArray(char * element, cJSON * array);

void generateToken(char * username);
void getReady(void);
void putFileContent(char * fileAddress, char * fileContent);
void increaseTweetCount(void);
void seeTweet(char * username, int tweetId);

int main() {
    getReady();

    int server_socket = initialize(PORT);
    if (server_socket != INVALID_SOCKET){
        start(server_socket);
    }
    return 0;
}

int initialize(int port){
    WSADATA wsadata;
    int wsa_res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (wsa_res != 0){
        printf("Failed to run wsadata");
        return INVALID_SOCKET;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Could not create socket");
        return INVALID_SOCKET;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int bind_res = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (bind_res == SOCKET_ERROR){
        printf("Bind Faild");
        return INVALID_SOCKET;
    }

    listen(server_socket, SOMAXCONN);

    return server_socket;

}

void start(int server_socket) {
    int len = sizeof(struct sockaddr_in);

    int client_socket;
    struct sockaddr_in client_addr;

    char *messages[SIZE];
    int size = 0;

    while((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &len)) != INVALID_SOCKET) {
        printf("Client connected\n");
        char *buffer;
        buffer = (char *)malloc(SIZE * sizeof(char));
        memset(buffer, 0, SIZE);
        recv(client_socket, buffer, SIZE-1, 0);

        char *response;
        response = (char *)malloc(SIZE * sizeof(char));

        printf("input: %s", buffer);
        strcpy(response, startGame(buffer));
        printf("output: %s\n\n", response);

        send(client_socket, response, strlen(response), 0);
        free(buffer);
        free(response);

        closesocket(client_socket);

    }
}

void getReady(void) {
    mkdir("resources");
    mkdir("resources\\users");
    mkdir("resources\\tweets");

    FILE * ptrToFile1 = fopen("resources\\usernameToToken.json", "w");
    FILE * ptrToFile2 = fopen("resources\\tokenToUsername.json", "w");
    FILE * ptrToFile3 = fopen("resources\\seenTweets.json", "w");

    cJSON * root = cJSON_CreateObject();

    fprintf(ptrToFile1, "%s", cJSON_PrintUnformatted(root));
    fprintf(ptrToFile2, "%s", cJSON_PrintUnformatted(root));
    fprintf(ptrToFile3, "%s", cJSON_PrintUnformatted(root));

    fclose(ptrToFile1);
    fclose(ptrToFile2);
    fclose(ptrToFile3);

    FILE * ptrToFile4 = fopen("resources\\tweetCount.json", "r");

    if (ptrToFile4 == NULL) {
        fclose(ptrToFile4);
        ptrToFile4 = fopen("resources\\tweetCount.json", "w");
        fprintf(ptrToFile4, "%d", 1);
        fclose(ptrToFile4);
    } else {
        fclose(ptrToFile4);
    }


}

char * startGame(char *input) {
    char *output;
    char command[50];
    sscanf(input, "%s", command);

    if(strcmp(command, "signup") == 0) {

        if(checkBadRequest(input, 2)) {output = generateError("Bad request format"); return output;}
        output = signup(input);

    } else if(strcmp(command, "login") == 0) {

        if(checkBadRequest(input, 2)) {output = generateError("Bad request format"); return output;}
        output = login(input);

    } else if(strcmp(command, "sendTweet") == 0) {

        output = sendTweet(input);

    } else if(strcmp(command, "refresh") == 0) {

        if(checkBadRequest(input, 1)) {output = generateError("Bad request format"); return output;}
        output = refresh(input);

    } else if(strcmp(command, "like") == 0) {

        if(checkBadRequest(input, 2)) {output = generateError("Bad request format"); return output;}
        output = like(input);

    } else if(strcmp(command, "comment") == 0) {

        output = comment(input);

    } else if(strcmp(command, "search") == 0) {

        if(checkBadRequest(input, 2)) {output = generateError("Bad request format"); return output;}
        output = search(input);

    } else if(strcmp(command, "follow") == 0) {

        if(checkBadRequest(input, 2)) {output = generateError("Bad request format"); return output;}
        output = follow(input);

    } else if(strcmp(command, "unfollow") == 0) {

        if(checkBadRequest(input, 2)) {output = generateError("Bad request format"); return output;}
        output = unfollow(input);

    } else if(strcmp(command, "profile") == 0) {

        if(checkBadRequest(input, 1)) {output = generateError("Bad request format"); return output;}
        output = profile(input);

    } else if(strcmp(command, "setBio") == 0) {

        output = setBio(input);

    } else if(strcmp(command, "changePassword") == 0) {

        if(checkBadRequest(input, 3)) {output = generateError("Bad request format"); return output;}
        output = changePassword(input);

    } else if(strcmp(command, "logout") == 0) {

        if(checkBadRequest(input, 1)) {output = generateError("Bad request format"); return output;}
        output = logout(input);

    } else {

        output = generateError("Bad request format (invalid command)");

    }
    return output;
}

char * signup(char *input) {
    char * output;
    char command[50], username[50], password[50];
    sscanf(input, "%s %s %s", command, username, password);

    char fileAddress[200];
    sprintf(fileAddress, "resources\\users\\%s.json", username);

    if (isThisUserThere(fileAddress)) {

        output = generateError("This username is already taken.");

    } else if (strlen(password) > 16) {

        output = generateError("please enter less than 16 characters for password");
        return output;

    } else if (strlen(username) > 32) {

        output = generateError("please enter less than 32 characters for username");
        return output;

    } else {

        cJSON * fileContent = cJSON_CreateObject();
        cJSON_AddStringToObject(fileContent, "username", username);
        cJSON_AddStringToObject(fileContent, "password", password);
        cJSON_AddStringToObject(fileContent, "bio", "");
        cJSON_AddArrayToObject(fileContent, "followers");
        cJSON_AddArrayToObject(fileContent, "followings");
        cJSON_AddArrayToObject(fileContent, "personalTweets");
        cJSON_AddArrayToObject(fileContent, "likedTweets");

        putFileContent(fileAddress, cJSON_PrintUnformatted(fileContent));
        output = generateSuccessful("");
    }

    return output;
}

char * login(char *input) {
    char * output;
    char command[50], username[50], password[50];
    sscanf(input, "%s %s %s", command, username, password);

    char fileAddress[200];
    sprintf(fileAddress, "resources\\users\\%s.json", username);

    cJSON * responseContent = cJSON_CreateObject();

    if (isThisUserThere(fileAddress)) {

        char * fileStringContent = getFileContent(fileAddress);
        cJSON * fileContent = cJSON_Parse(fileStringContent);
        cJSON * pass = cJSON_GetObjectItem(fileContent, "password");

        if (strcmp(password, pass->valuestring) == 0) {

            if (isThisUserLoggedIn(username)) {

                output = generateError("The user is already logged in.");

            } else {
                generateToken(username);

                cJSON * toToUs = cJSON_Parse(getFileContent("resources\\tokenToUsername.json"));
                cJSON * usToTo = cJSON_Parse(getFileContent("resources\\usernameToToken.json"));
                cJSON * seenTweets = cJSON_Parse(getFileContent("resources\\seenTweets.json"));

                cJSON_AddStringToObject(toToUs, globalToken, username);
                cJSON_AddStringToObject(usToTo, username, globalToken);
                cJSON_AddArrayToObject(seenTweets, username);

                putFileContent("resources\\tokenToUsername.json", cJSON_PrintUnformatted(toToUs));
                putFileContent("resources\\usernameToToken.json", cJSON_PrintUnformatted(usToTo));
                putFileContent("resources\\seenTweets.json", cJSON_PrintUnformatted(seenTweets));

                cJSON_AddStringToObject(responseContent, "type", "Token");
                cJSON_AddStringToObject(responseContent, "message", globalToken);
                output = cJSON_PrintUnformatted(responseContent);

                strcpy(globalToken, "");
            }

        } else {

            output = generateError("Wrong password!");

        }

    } else {

        output = generateError("This username is not valid.");

    }

    return output;
}

char * sendTweet(char *input) {
    char * output;
    char command[50], token[100], tweetTex[300];
    sscanf(input, "%s %s %[^\n]s", command, token, tweetTex);

    char * username = tokenToUsername(token);

    if (strcmp(username, "0") == 0) {

        output = generateError("Bad request format! (invalid token)");

    } else {

        char profileFileAddress[100], tweetFileAddress[100];
        sprintf(profileFileAddress, "resources\\users\\%s.json", username);
        sprintf(tweetFileAddress, "resources\\tweets\\%d.json", getTweetCount());

        char * profileText = getFileContent(profileFileAddress);
        cJSON * profileContent = cJSON_Parse(profileText);
        cJSON * tweets = cJSON_GetObjectItem(profileContent, "personalTweets");
        cJSON * tweetId = cJSON_CreateNumber(getTweetCount());
        cJSON_AddItemToArray(tweets, tweetId);
        putFileContent(profileFileAddress, cJSON_PrintUnformatted(profileContent));

        cJSON * generateTweet = cJSON_CreateObject();
        cJSON_AddNumberToObject(generateTweet, "id", getTweetCount());
        cJSON_AddStringToObject(generateTweet, "author", username);
        cJSON_AddStringToObject(generateTweet, "content", tweetTex);
        cJSON_AddObjectToObject(generateTweet, "comments");
        cJSON_AddNumberToObject(generateTweet, "likes", 0);
        putFileContent(tweetFileAddress, cJSON_PrintUnformatted(generateTweet));

        increaseTweetCount();
        output = generateSuccessful("Tweet is sent successfully.");

    }

    return output;
}

char * refresh(char *input) {
    char * output;
    char command[50], token[100];
    sscanf(input, "%s %s", command, token);

    char * username = tokenToUsername(token);
    char profileFileAddress[150];
    sprintf(profileFileAddress, "resources\\users\\%s.json", username);

    if(strcmp(username, "0") == 0) {

        output = generateError("Bad request format (invalid token)");
        return output;

    }

    cJSON * responseContent = cJSON_CreateObject();
    cJSON_AddStringToObject(responseContent, "type", "List");
    cJSON_AddArrayToObject(responseContent, "message");
    cJSON * arr = cJSON_GetObjectItem(responseContent, "message");

    cJSON * profileContent = cJSON_Parse(giveMeProfile(username));
    cJSON * followings = cJSON_GetObjectItem(profileContent, "followings");

    cJSON *hisProfile, *hisTweets, *newTweet, *eachTweet, *eachAcc, *number;

    eachAcc = NULL;
    cJSON_ArrayForEach(eachAcc, followings) {
        hisProfile = cJSON_Parse(giveMeProfile(eachAcc->valuestring));
        hisTweets = cJSON_GetObjectItem(hisProfile, "personalTweets");

        eachTweet = NULL;
        cJSON_ArrayForEach(eachTweet, hisTweets) {
            if(didYouSeeThat(username, eachTweet->valueint) == 0) {
                newTweet = cJSON_Parse(giveMeTweet(eachTweet->valueint));
                cJSON_AddItemToArray(arr, newTweet);
                seeTweet(username, eachTweet->valueint);
            }
        }
    }

    output = cJSON_PrintUnformatted(responseContent);
    return output;
}

char * like(char *input) {
    char * output;
    char command[50], token[100];
    int tweetId;
    sscanf(input, "%s %s %d", command, token, &tweetId);

    char * username = tokenToUsername(token);
    if(strcmp(username, "0") == 0) {

        output = generateError("Bad request format (invalid token)");
        return output;

    }

    if(didYouLikeIt(username, tweetId) == 0) {
        if(strcmp(giveMeTweet(tweetId), "0") == 0) {

            output = generateError("Bad request format (invalid tweet ID)");
            return output;

        }
        cJSON * tweetContent = cJSON_Parse(giveMeTweet(tweetId));
        cJSON * likes = cJSON_GetObjectItem(tweetContent, "likes");
        int likesNow = likes->valueint + 1;

        cJSON * num = cJSON_CreateNumber(likesNow);

        cJSON_ReplaceItemInObject(tweetContent, "likes", num);

        char tweetAddress[100];
        sprintf(tweetAddress, "resources\\tweets\\%d.json", tweetId);
        putFileContent(tweetAddress, cJSON_PrintUnformatted(tweetContent));

        cJSON  * profileContent = cJSON_Parse(giveMeProfile(username));
        cJSON * likedTweets = cJSON_GetObjectItem(profileContent, "likedTweets");
        cJSON * number = cJSON_CreateNumber(tweetId);
        cJSON_AddItemToArray(likedTweets, number);

        char profileFileAddress[200];
        sprintf(profileFileAddress, "resources\\users\\%s.json", username);
        putFileContent(profileFileAddress, cJSON_PrintUnformatted(profileContent));

        char message[50];
        sprintf(message, "Tweet %d successfully liked.", tweetId);

        output = generateSuccessful(message);

        return output;
    } else {

        output = generateError("This tweet is already liked");

        return output;
    }

}

char * comment(char *input) {
    char * output;
    char command[50], token[100], commentText[100];
    int tweetId;
    sscanf(input, "%s %s %d %[^\n]s", command, token, &tweetId, commentText);

    char * username = tokenToUsername(token);

    if(strcmp(username, "0") == 0) {

        output = generateError("Bad request format (invalid token)");
        return output;

    } else if(strcmp(giveMeTweet(tweetId), "0") == 0) {

        output = generateError("Bad request format (invalid tweet ID)");
        return output;

    }

    cJSON * tweetContent = cJSON_Parse(giveMeTweet(tweetId));
    cJSON * comments = cJSON_GetObjectItem(tweetContent, "comments");
    cJSON_AddStringToObject(comments, username, commentText);

    char tweetFileAddress[100];
    sprintf(tweetFileAddress, "resources\\tweets\\%d.json", tweetId);
    putFileContent(tweetFileAddress, cJSON_PrintUnformatted(tweetContent));

    output = generateSuccessful("Your comment was sent successfully");

    return output;
}

char * search(char *input) {
    char * output;
    char command[50], token[100], username[50];
    sscanf(input, "%s %s %s", command, token, username);

    char * profileContent = giveMeProfile(username);

    if(strcmp(tokenToUsername(token), username) == 0) {

        output = generateError("Searched username is your username! you f...ing donkey!");
        return output;

    } else if(strcmp(profileContent, "0") == 0) {

        output = generateError("This username is not valid.");
        return output;

    } else if(strcmp(tokenToUsername(token), "0") == 0) {

        output = generateError("Bad request format! (invalid token)");
        return output;

    } else {

        cJSON * responseContent = cJSON_CreateObject();
        cJSON_AddStringToObject(responseContent, "type", "Profile");

        cJSON_AddObjectToObject(responseContent, "message");
        cJSON * message = cJSON_GetObjectItem(responseContent, "message");
        cJSON * profile = cJSON_Parse(giveMeProfile(username));
        cJSON * bio = cJSON_GetObjectItem(profile, "bio");

        cJSON_AddStringToObject(message, "username", username);
        cJSON_AddStringToObject(message, "bio", bio->valuestring);
        cJSON_AddNumberToObject(message, "numberOfFollowers", numOfFollowers(username));
        cJSON_AddNumberToObject(message, "numberOfFollowings", numOfFollowings(username));
        cJSON_AddStringToObject(message, "followStatus", didYouFollowHim(tokenToUsername(token), username) ? "Followed" : "NotFollowed");
        cJSON_AddArrayToObject(message, "allTweets");
        cJSON * allTweetsArray = cJSON_GetObjectItem(message, "allTweets");

        cJSON * listOfTweets = cJSON_GetObjectItem(profile, "personalTweets");

        cJSON * eachTweetId = NULL;
        cJSON_ArrayForEach(eachTweetId, listOfTweets) {
            cJSON * tweetContent = cJSON_Parse(giveMeTweet(eachTweetId->valueint));
            cJSON_AddItemToArray(allTweetsArray, tweetContent);
        }

        output = cJSON_PrintUnformatted(responseContent);

        return output;
    }
}

char * follow(char *input) {
    char * output;
    char command[50], token[100], username[50];
    sscanf(input, "%s %s %s", command, token, username);

    if(strcmp(tokenToUsername(token), "0") == 0) {

        output = generateError("Bad request format! (invalid token)");
        return output;

    } else if(strcmp(giveMeProfile(username), "0") == 0) {

        output = generateError("This username is not valid.");
        return output;

    } else if(didYouFollowHim(tokenToUsername(token), username)) {

        output = generateError("You have already followed this user.");
        return output;

    } else if(strcmp(tokenToUsername(token), username) == 0) {

        output = generateError("You can not follow YOURSELF!!!");
        return output;

    }

    char faelProfileAddress[150], mafoolProfileAddress[150];
    sprintf(faelProfileAddress, "resources\\users\\%s.json", tokenToUsername(token));
    sprintf(mafoolProfileAddress, "resources\\users\\%s.json", username);

    cJSON * faelProfileContent = cJSON_Parse(giveMeProfile(tokenToUsername(token)));
    cJSON * faelFollowings = cJSON_GetObjectItem(faelProfileContent, "followings");
    cJSON * string = cJSON_CreateString(username);
    cJSON_AddItemToArray(faelFollowings, string);
    putFileContent(faelProfileAddress, cJSON_PrintUnformatted(faelProfileContent));

    cJSON * mafoolProfileContent = cJSON_Parse(giveMeProfile(username));
    cJSON * mafoolFollowings = cJSON_GetObjectItem(mafoolProfileContent, "followers");
    string = cJSON_CreateString(tokenToUsername(token));
    cJSON_AddItemToArray(mafoolFollowings, string);
    putFileContent(mafoolProfileAddress, cJSON_PrintUnformatted(mafoolProfileContent));

    char message[200];
    sprintf(message, "User %s is followed successfully.", username);
    output = generateSuccessful(message);

    return output;
}

char * unfollow(char *input) {
    char * output;
    char command[50], token[100], username[50];
    sscanf(input, "%s %s %s", command, token, username);

    if(strcmp(tokenToUsername(token), "0") == 0) {

        output = generateError("Bad request format! (invalid token)");
        return output;

    } else if(strcmp(giveMeProfile(username), "0") == 0) {

        output = generateError("This username is not valid.");
        return output;

    } else if(didYouFollowHim(tokenToUsername(token), username) == 0) {

        output = generateError("You have not followed this user before.");
        return output;

    } else if(strcmp(tokenToUsername(token), username) == 0) {

        output = generateError("You can not unfollow YOURSELF!!!");
        return output;

    }

    char faelProfileAddress[150], mafoolProfileAddress[150];
    sprintf(faelProfileAddress, "resources\\users\\%s.json", tokenToUsername(token));
    sprintf(mafoolProfileAddress, "resources\\users\\%s.json", username);

    cJSON * faelProfileContent = cJSON_Parse(giveMeProfile(tokenToUsername(token)));
    cJSON * faelFollowings = cJSON_GetObjectItem(faelProfileContent, "followings");
    cJSON_DeleteItemFromArray(faelFollowings, findIndexInArray(username, faelFollowings));
    putFileContent(faelProfileAddress, cJSON_PrintUnformatted(faelProfileContent));

    cJSON * mafoolProfileContent = cJSON_Parse(giveMeProfile(username));
    cJSON * mafoolFollowers = cJSON_GetObjectItem(mafoolProfileContent, "followers");
    cJSON_DeleteItemFromArray(mafoolFollowers, findIndexInArray(tokenToUsername(token), mafoolFollowers));
    putFileContent(mafoolProfileAddress, cJSON_PrintUnformatted(mafoolProfileContent));

    char message[200];
    sprintf(message, "User %s is unfollowed successfully.", username);
    output = generateSuccessful(message);

    return output;
}

char * profile(char *input) {
    char * output;
    char command[50], token[100];
    sscanf(input, "%s %s", command, token);

    char * username = tokenToUsername(token);
    char * profileContent = giveMeProfile(username);

    if(strcmp(username, "0") == 0) {

        output = generateError("Bad request format! (invalid token)");
        return output;

    } else {

        cJSON * responseContent = cJSON_CreateObject();
        cJSON_AddStringToObject(responseContent, "type", "Profile");

        cJSON_AddObjectToObject(responseContent, "message");
        cJSON * message = cJSON_GetObjectItem(responseContent, "message");
        cJSON * profile = cJSON_Parse(profileContent);

        cJSON_AddStringToObject(message, "username", username);
        cJSON_AddStringToObject(message, "bio", (cJSON_GetObjectItem(profile, "bio"))->valuestring);
        cJSON_AddNumberToObject(message, "numberOfFollowers", numOfFollowers(username));
        cJSON_AddNumberToObject(message, "numberOfFollowings", numOfFollowings(username));
        cJSON_AddStringToObject(message, "followStatus", "Yourself");
        cJSON_AddArrayToObject(message, "allTweets");
        cJSON * allTweetsArray = cJSON_GetObjectItem(message, "allTweets");

        cJSON * listOfTweets = cJSON_GetObjectItem(profile, "personalTweets");
        cJSON * eachTweetId = NULL;
        cJSON_ArrayForEach(eachTweetId, listOfTweets) {
            cJSON * tweetContent = cJSON_Parse(giveMeTweet(eachTweetId->valueint));
            cJSON_AddItemToArray(allTweetsArray, tweetContent);
        }

        output = cJSON_PrintUnformatted(responseContent);
    }

    return output;
}

char * setBio(char *input) {
    char * output;
    char command[50], token[100], bioText[150];
    sscanf(input, "%s %s %[^\n]s", command, token, bioText);

    char * username = tokenToUsername(token);

    char profileAddress[150];
    sprintf(profileAddress, "resources\\users\\%s.json", username);

    if(strcmp(username, "0") == 0) {

        output = generateError("Bad request format! (invalid token)");
        return output;

    } else {

        cJSON * profileContent = cJSON_Parse(giveMeProfile(username));
        cJSON * bioTocJSONString = cJSON_CreateString(bioText);
        cJSON_ReplaceItemInObject(profileContent, "bio", bioTocJSONString);
        putFileContent(profileAddress, cJSON_PrintUnformatted(profileContent));

        output = generateSuccessful("Your bio has changed successfully.");
        return output;

    }
}

char * changePassword(char *input) {
    char *output;
    char command[50], token[100], oldPass[100], pass[100];
    sscanf(input, "%s %s %s %s", command, token, oldPass, pass);

    char *username = tokenToUsername(token);

    char profileAddress[150];
    sprintf(profileAddress, "resources\\users\\%s.json", username);

    cJSON *profileContent = cJSON_Parse(giveMeProfile(username));
    cJSON *oldPassword = cJSON_GetObjectItem(profileContent, "password");

    if (strcmp(username, "0") == 0) {

        output = generateError("Bad request format! (invalid token)");
        return output;

    } else if (strcmp(oldPass, oldPassword->valuestring) != 0) {

        output = generateError("Entered current password is wrong.");
        return output;

    } else if (strlen(pass) > 16) {

        output = generateError("please enter less than 16 characters");
        return output;

    } else if(strcmp(oldPassword->valuestring, pass) == 0) {

        output = generateSuccessful("Your old and new password are identical.");
        return output;

    } else {

        cJSON * newPassTocJsonString = cJSON_CreateString(pass);
        cJSON_ReplaceItemInObject(profileContent, "password", newPassTocJsonString);

        putFileContent(profileAddress, cJSON_PrintUnformatted(profileContent));

        output = generateSuccessful("Your password has changed successfully");
    }

    return output;
}

char * logout(char *input) {
    char * output;
    char command[50], token[100];
    sscanf(input, "%s %s", command, token);

    char * username = tokenToUsername(token);

    if(strcmp(tokenToUsername(token), "0") == 0) {
        output = generateError("Bad request format! (invalid token)");
        return output;

    } else {
        FILE * ptrToFile1 = fopen("resources\\usernameToToken.json", "r");
        FILE * ptrToFile2 = fopen("resources\\tokenToUsername.json", "r");
        FILE * ptrToFile3 = fopen("resources\\seenTweets.json", "r");

        char fileContent1[100000];
        char fileContent2[100000];
        char fileContent3[100000];

        fscanf(ptrToFile1, "%[^\n]s", fileContent1);
        fscanf(ptrToFile2, "%[^\n]s", fileContent2);
        fscanf(ptrToFile3, "%[^\n]s", fileContent3);

        fclose(ptrToFile1);
        fclose(ptrToFile2);
        fclose(ptrToFile3);

        cJSON * usToTo = cJSON_Parse(fileContent1);
        cJSON * toToUs = cJSON_Parse(fileContent2);
        cJSON * seenTweets = cJSON_Parse(fileContent3);

        cJSON_DeleteItemFromObject(usToTo, username);
        cJSON_DeleteItemFromObject(toToUs, token);
        cJSON_DeleteItemFromObject(seenTweets, username);

        putFileContent("resources\\usernameToToken.json", cJSON_PrintUnformatted(usToTo));
        putFileContent("resources\\tokenToUsername.json", cJSON_PrintUnformatted(toToUs));
        putFileContent("resources\\seenTweets.json", cJSON_PrintUnformatted(seenTweets));

        output = generateSuccessful("");
        return output;
    }
}

void generateToken(char * username) {
    int changeStartRand = 0;

    for (int i = 0; i<strlen(username); i++) {
        changeStartRand += username[i];
    }

    for (int j = 0; j<32; j++) {

        srand(time(NULL) + changeStartRand + j);

        if(rand() % 3 == 0) {

            globalToken[j] = 'A' + (rand() % 26);

        } else if(rand() % 3 == 1) {

            globalToken[j] = 'a' + (rand() % 26);

        } else {

            globalToken[j] = '0' + (rand() % 10);

        }
    }
}

void putFileContent(char * fileAddress, char * fileContent) {
    FILE * ptrToFile = fopen(fileAddress, "w");

    fprintf(ptrToFile, "%s", fileContent);

    fclose(ptrToFile);
}

char * getFileContent(char * fileAddress) {
    FILE * ptrToFile = fopen(fileAddress, "r");

    char fileContent[1000000];

    fscanf(ptrToFile, "%[^\n]s", fileContent);

    char * output = fileContent;
    fclose(ptrToFile);

    return output;
}

int isThisUserThere(char * fileAddress) {
    FILE * ptrToFile = fopen(fileAddress, "r");

    FILE * savePtr = ptrToFile;
    fclose(ptrToFile);

    return savePtr == NULL ? 0 : 1;
}

int isThisUserLoggedIn(char * username) {
    char fileStringContent[1000000];

    FILE * ptrToFile = fopen("resources\\usernameToToken.json", "r");
    fscanf(ptrToFile, "%[^\n]s", fileStringContent);
    fclose(ptrToFile);

    cJSON * fileContent = cJSON_Parse(fileStringContent);

    return cJSON_GetObjectItem(fileContent, username) ? 1 : 0;
}

char * tokenToUsername(char * token) {
    cJSON * root = cJSON_Parse(getFileContent("resources\\tokenToUsername.json"));
    char * output;

    if (cJSON_GetObjectItem(root, token)) {
        cJSON  * username = cJSON_GetObjectItem(root, token);
        output = username->valuestring;
        return output;
    } else {
        output = "0";
        return output;
    }

}

int getTweetCount(void) {
    FILE * ptrToFile = fopen("resources\\tweetCount.json", "r");

    int answer;
    fscanf(ptrToFile, "%d", &answer);

    fclose(ptrToFile);
    return answer;
}

void increaseTweetCount(void) {
    int countNow = getTweetCount();

    FILE * ptrToFile = fopen("resources\\tweetCount.json", "w");

    fprintf(ptrToFile, "%d", countNow+1);

    fclose(ptrToFile);
}

char * giveMeProfile(char * username) {
    char profileFileAddress[100], profileContent[1000000];
    sprintf(profileFileAddress, "resources\\users\\%s.json", username);

    char * output;

    FILE * ptrToFile = fopen(profileFileAddress, "r");

    if(ptrToFile != NULL) {
        fscanf(ptrToFile, "%[^\n]s", profileContent);
        fclose(ptrToFile);
        output = profileContent;
    } else {
        output = "0";
    }

    return output;
}

char * giveMeTweet(int id) {
    char fileAddress[100];
    sprintf(fileAddress, "resources\\tweets\\%d.json", id);

    FILE * ptrToFile = fopen(fileAddress, "r");

    if(ptrToFile != NULL) {

        char tweetInfo[100000];
        fscanf(ptrToFile, "%[^\n]s", tweetInfo);

        fclose(ptrToFile);

        char * output = tweetInfo;
        return output;

    } else {

        char * output = "0";
        return output;

    }
}

void seeTweet(char * username, int tweetId) {
    cJSON * seenTweets = cJSON_Parse(getFileContent("resources\\seenTweets.json"));
    cJSON * mySeenTweets = cJSON_GetObjectItem(seenTweets, username);
    cJSON * number = cJSON_CreateNumber(tweetId);

    cJSON_AddItemToArray(mySeenTweets, number);

    putFileContent("resources\\seenTweets.json", cJSON_PrintUnformatted(seenTweets));
}

int didYouSeeThat(char * username, int tweetId) {
    int output = 0;
    cJSON * seenTweets = cJSON_Parse(getFileContent("resources\\seenTweets.json"));
    cJSON * mySeenTweets = cJSON_GetObjectItem(seenTweets, username);

    cJSON *eachTweet = NULL;
    cJSON_ArrayForEach(eachTweet, mySeenTweets) {
        if (tweetId == eachTweet->valueint) {
            output = 1;
        }
    }

    return output;
}

int didYouLikeIt(char * username, int tweetId) {
    int output = 0;
    cJSON * profileContent = cJSON_Parse(giveMeProfile(username));
    cJSON * likedTweets = cJSON_GetObjectItem(profileContent, "likedTweets");

    cJSON *eachTweet = NULL;
    cJSON_ArrayForEach(eachTweet, likedTweets) {
        if (tweetId == eachTweet->valueint) {
            output = 1;
        }
    }

    return output;
}

char * generateError(char * errorText) {
    cJSON * responseContent = cJSON_CreateObject();
    cJSON_AddStringToObject(responseContent, "type", "Error");
    cJSON_AddStringToObject(responseContent, "message", errorText);

    char * output = cJSON_PrintUnformatted(responseContent);
    return output;
}

char * generateSuccessful(char * messageText) {
    cJSON * responseContent = cJSON_CreateObject();
    cJSON_AddStringToObject(responseContent, "type", "Successful");
    cJSON_AddStringToObject(responseContent, "message", messageText);

    char * output = cJSON_PrintUnformatted(responseContent);
    return output;
}

int numOfFollowers(char * username) {
    cJSON * profileContent = cJSON_Parse(giveMeProfile(username));
    cJSON * followersArray =  cJSON_GetObjectItem(profileContent, "followers");

    int answer = 0;
    cJSON * eachFollower = NULL;
    cJSON_ArrayForEach(eachFollower, followersArray) {
        answer++;
    }

    return answer;
}

int numOfFollowings(char * username) {
    cJSON * profileContent = cJSON_Parse(giveMeProfile(username));
    cJSON * followersArray =  cJSON_GetObjectItem(profileContent, "followings");

    int answer = 0;
    cJSON * eachFollowing = NULL;
    cJSON_ArrayForEach(eachFollowing, followersArray) {
        answer++;
    }

    return answer;
}

int didYouFollowHim(char * fael, char * mafool) {
    cJSON * profileContent = cJSON_Parse(giveMeProfile(fael));
    cJSON * followingsArray  = cJSON_GetObjectItem(profileContent, "followings");

    cJSON * eachAcc = NULL;
    cJSON_ArrayForEach(eachAcc, followingsArray) {
        if(strcmp(eachAcc->valuestring, mafool) == 0) {
            return 1;
        }
    }

    return 0;
}

int findIndexInArray(char * element, cJSON * array) {
    int index = 0;

    cJSON * eachElement = NULL;
    cJSON_ArrayForEach(eachElement, array) {
        if(strcmp(eachElement->valuestring, element) == 0) {
            return index;
        }
        index++;
    }
}

int checkBadRequest(char * input, int legalSpaces) {
    int length = strlen(input);
    int countSpaces = 0;

    for (int i = 0; i<length; i++) {
        if(input[i] == ' ' && isalnum(input[i-1]) && isalnum(input[i+1])) {
            countSpaces += 1;
        }
    }

    return legalSpaces == countSpaces ? 0 : 1;
}