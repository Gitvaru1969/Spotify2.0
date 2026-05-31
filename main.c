#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* -------------------------------------------------------------------------
 * Song - represents a single track in the playlist
 * ------------------------------------------------------------------------- */
typedef struct {
    char title[64];    /* song title  */
    char artist[64];   /* artist name */
    int  duration;     /* length in seconds */
} Song;

/* -------------------------------------------------------------------------
 * makeSong - helper: allocate and return a new Song
 * ------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------
 * printSong - print a formatted song entry
 * ------------------------------------------------------------------------- */
static void printSong(int num, Song *sp)
{
    int mins = sp->duration / 60;
    int secs = sp->duration % 60;
    printf("  %2d. %-30s %-25s %d:%02d\n", num, sp->title, sp->artist, mins, secs);
}

/* -------------------------------------------------------------------------
 * printPlaylist - display all songs currently in the playlist
 * O(n) - visits every item
 * ------------------------------------------------------------------------- */
static void printPlaylist(LIST *lp)
{
    int total = numItems(lp);
    printf("\n========== SPOTIFY PLAYLIST (%d songs) ==========\n", total);
    printf("  %-3s %-30s %-25s %s\n", "#", "Title", "Artist", "Duration");
    printf("  -----------------------------------------------------------\n");
    for (int i = 0; i < total; i++) {
        Song *sp = (Song *)getItem(lp, i);
        printSong(i + 1, sp);
    }
    printf("=================================================\n\n");
}

/* -------------------------------------------------------------------------
 * main - demonstrates the playlist using all list operations
 * ------------------------------------------------------------------------- */
int main(void)
{
    LIST *playlist = createList();

    printf(">>> Creating playlist and queueing songs to the end (addLast)...\n");

    /* Queue songs to the back of the playlist (addLast) */
    addLast(playlist, makeSong("Blinding Lights",     "The Weeknd",       200));
    addLast(playlist, makeSong("Levitating",          "Dua Lipa",         203));
    addLast(playlist, makeSong("Stay",                "The Kid LAROI",    141));
    addLast(playlist, makeSong("Peaches",             "Justin Bieber",    198));
    addLast(playlist, makeSong("Good 4 U",            "Olivia Rodrigo",   178));
    addLast(playlist, makeSong("Montero",             "Lil Nas X",        137));
    addLast(playlist, makeSong("drivers license",     "Olivia Rodrigo",   242));
    addLast(playlist, makeSong("Save Your Tears",     "The Weeknd",       215));
    addLast(playlist, makeSong("Butter",              "BTS",              164));
    addLast(playlist, makeSong("Industry Baby",       "Lil Nas X",        212));

    printPlaylist(playlist);

    /* --- addFirst: bump a song to play next --- */
    printf(">>> Bumping 'Heat Waves' to the front (addFirst - play next)...\n");
    addFirst(playlist, makeSong("Heat Waves", "Glass Animals", 238));
    printPlaylist(playlist);

    /* --- removeFirst: play (consume) the first song --- */
    printf(">>> Now playing and removing the first song (removeFirst)...\n");
    Song *nowPlaying = (Song *)removeFirst(playlist);
    printf("  Now Playing: %s by %s\n\n", nowPlaying->title, nowPlaying->artist);
    free(nowPlaying);

    /* --- removeLast: drop the last song from the queue --- */
    printf(">>> Removing the last song from the queue (removeLast)...\n");
    Song *dropped = (Song *)removeLast(playlist);
    printf("  Removed: %s by %s\n\n", dropped->title, dropped->artist);
    free(dropped);

    /* --- getItem: jump directly to track by index --- */
    printf(">>> Jumping directly to track #5 (getItem at index 4)...\n");
    Song *track5 = (Song *)getItem(playlist, 4);
    printf("  Track 5: %s by %s\n\n", track5->title, track5->artist);

    /* --- numItems: check how many songs remain --- */
    printf(">>> Songs remaining in playlist: %d\n\n", numItems(playlist));

    printPlaylist(playlist);

    /* --- Clean up: free all remaining songs and the list --- */
    printf(">>> Clearing playlist and freeing all memory...\n");
    while (numItems(playlist) > 0) {
        Song *sp = (Song *)removeFirst(playlist);
        free(sp);
    }
    destroyList(playlist);

    printf("Playlist destroyed. All memory freed.\n");
    return 0;
}
