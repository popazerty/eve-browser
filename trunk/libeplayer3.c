#ifdef __sh__

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "libeplayer3/common.h"
#include "libeplayer3/subtitle.h"

Context_t * player;

extern OutputHandler_t		OutputHandler;
extern PlaybackHandler_t	PlaybackHandler;
extern ContainerHandler_t	ContainerHandler;
extern ManagerHandler_t		ManagerHandler;


void play(char * file)
{
    player = (Context_t*) malloc(sizeof(Context_t));

    if(player) {
	    player->playback	= &PlaybackHandler;
	    player->output		= &OutputHandler;
	    player->container	= &ContainerHandler;
	    player->manager		= &ManagerHandler;

    	printf("%s\n", player->output->Name);
    }

	//Registration of output devices
    if(player && player->output) {
	    player->output->Command(player,OUTPUT_ADD, (void*)"audio");
	    player->output->Command(player,OUTPUT_ADD, (void*)"video");
	    player->output->Command(player,OUTPUT_ADD, (void*)"subtitle");
    }

    //try to open file
	if(player && player->playback && player->playback->Command(player, PLAYBACK_OPEN, file) >= 0) {

	    if(player && player->output && player->playback) {
            player->output->Command(player, OUTPUT_OPEN, NULL);
            player->playback->Command(player, PLAYBACK_PLAY, NULL);
        }

    } else {
        //Creation failed, no playback support for insert file, so delete playback context

        //FIXME: How to tell e2 that we failed?

        if(player && player->output) {
            player->output->Command(player,OUTPUT_DEL, (void*)"audio");
	        player->output->Command(player,OUTPUT_DEL, (void*)"video");
	        player->output->Command(player,OUTPUT_DEL, (void*)"subtitle");
        }

        if(player && player->playback)
            player->playback->Command(player,PLAYBACK_CLOSE, NULL);

        if(player)
            free(player);
        player = NULL;

    }
}

#endif

