#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Song - represents a single track in the playlist
typedef struct {
    char title[64];   // song title
    char artist[64];  // artist name
    int  duration;    // length in seconds
} Song;

// makeSong - allocate and return a new Song // O(1)
static Song *makeSong(const char *title, const char *artist, int duration)
{
    Song *sp = malloc(sizeof(Song));
    if (sp == NULL) {
        fprintf(stderr, "Error: malloc failed in makeSong\n");
        exit(1);
    }
    strncpy(sp->title,  title,  sizeof(sp->title)  - 1);
    strncpy(sp->artist, artist, sizeof(sp->artist) - 1);
    sp->duration = duration;
    return sp;
}

// printSong - print one formatted song row // O(1)
static void printSong(int num, Song *sp)
{
    int mins = sp->duration / 60;
    int secs = sp->duration % 60;
    printf("  %2d. %-30s %-25s %d:%02d\n", num, sp->title, sp->artist, mins, secs);
}

// printPlaylist - display all songs in the playlist // O(n)
static void printPlaylist(LIST *lp)
{
    int total = numItems(lp);
    printf("\n========== SPOTIFY PLAYLIST (%d songs) ==========\n", total);
    printf("  %-3s %-30s %-25s %s\n", "#", "Title", "Artist", "Duration");
    printf("  -----------------------------------------------------------\n");
    for (int i = 0; i < total; i++) {
        Song *sp = (Song *)getItem(lp, i); // getItem is O(n/m) per call
        printSong(i + 1, sp);
    }
    printf("=================================================\n\n");
}

// promptSong - prompt user to enter song details and return a new Song // O(1)
static Song *promptSong(void)
{
    char title[64], artist[64];
    int  duration;

    printf("  Title:           ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0'; // strip trailing newline

    printf("  Artist:          ");
    fgets(artist, sizeof(artist), stdin);
    artist[strcspn(artist, "\n")] = '\0';

    printf("  Duration (secs): ");
    scanf("%d", &duration);
    getchar(); // consume leftover newline after scanf

    return makeSong(title, artist, duration);
}

// main - interactive Spotify-style playlist manager
int main(void)
{
    LIST *playlist = createList();
    int   choice;

    printf("=== Spotify Playlist Manager ===\n");

    while (1) {
        // print menu options
        printf("\n1. View playlist\n");
        printf("2. Add song to end (queue)\n");
        printf("3. Add song to front (play next)\n");
        printf("4. Play next song (remove from front)\n");
        printf("5. Remove last song\n");
        printf("6. Jump to track by number\n");
        printf("7. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        if (choice == 1) {
            // view playlist
            if (numItems(playlist) == 0)
                printf("Playlist is empty.\n");
            else
                printPlaylist(playlist);

        } else if (choice == 2) {
            // add song to end via addLast // O(1)
            printf("Enter song details:\n");
            Song *sp = promptSong();
            addLast(playlist, sp);
            printf("Added '%s' to the end.\n", sp->title);

        } else if (choice == 3) {
            // bump song to front via addFirst // O(1)
            printf("Enter song details:\n");
            Song *sp = promptSong();
            addFirst(playlist, sp);
            printf("'%s' will play next.\n", sp->title);

        } else if (choice == 4) {
            // play and remove the front song via removeFirst // O(1)
            if (numItems(playlist) == 0) {
                printf("Playlist is empty.\n");
            } else {
                Song *sp = (Song *)removeFirst(playlist);
                printf("Now Playing: %s by %s\n", sp->title, sp->artist);
                free(sp);
            }

        } else if (choice == 5) {
            // remove the last song via removeLast // O(1)
            if (numItems(playlist) == 0) {
                printf("Playlist is empty.\n");
            } else {
                Song *sp = (Song *)removeLast(playlist);
                printf("Removed '%s' from the end.\n", sp->title);
                free(sp);
            }

        } else if (choice == 6) {
            // jump to a track by index via getItem // O(n/m)
            if (numItems(playlist) == 0) {
                printf("Playlist is empty.\n");
            } else {
                int track;
                printf("Enter track number (1-%d): ", numItems(playlist));
                scanf("%d", &track);
                getchar();
                if (track < 1 || track > numItems(playlist)) {
                    printf("Invalid track number.\n");
                } else {
                    Song *sp = (Song *)getItem(playlist, track - 1);
                    printf("Track %d: %s by %s\n", track, sp->title, sp->artist);
                }
            }

        } else if (choice == 7) {
            // free all songs and destroy the list on exit
            while (numItems(playlist) > 0) {
                Song *sp = (Song *)removeFirst(playlist);
                free(sp);
            }
            destroyList(playlist);
            printf("Goodbye!\n");
            break;

        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
