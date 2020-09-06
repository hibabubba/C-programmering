#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "pgmread.h"




// Server side implementation of UDP client-server model 

  
//#define PORT     8080 
#define MAXLINE 3000 
struct Packet *packetpeker;
struct Packet *forstepeker = NULL;
int teller=0;

int PORT;
struct Packet{
	//header
	int totlengde;
	unsigned char sekvensnr;
	unsigned char sekack;
	unsigned char flagg; //bit forandring
	unsigned char unused;

	//payload
	int define;
	int lengde_filnavn;//med endelig 0
	char filnavn[30];//med endelig 0*/

	char *bilde;//malloc plass for char bildet
	struct Packet *neste;

}; 

void printpacket(struct Packet *pake){

   //printe pakken
      	 	printf("Printer totlengde %d\n",pake->totlengde);
		    printf(" Sekvensnr: %d\n", pake->sekvensnr);
		    printf(" Sekvensnr av siste motatte ack: %d\n", pake->sekack); 
		    printf(" FLagg til packeten er: %d\n", pake->flagg);
		    printf(" Unused til packeten er: %d\n", pake->unused);
		   printf(" Unikt debug nr : %d\n", pake->define);
		    printf(" Lengden på filnavn er: %d\n", pake->lengde_filnavn);
		    printf(" Filnavn til packet er: %s\n", pake->filnavn);
		    printf(" Bilde til packet er: %s\n", pake->bilde);

   
    printf("\n\n");} 

void sett_nestepeker(struct Packet* pake){
		
			
				      	 	//legge til struct peker neste
				      	 	
				      	 	if(pake->sekvensnr == 0 ){ // første i lenkelista
				      	 		packetpeker = pake;
				      	 		forstepeker = pake;
				      	 	}
				      	 	else{ 	 		
				      	 			//finn packetpeker->neste = null og sett den packeten sin neste til å være pakke
				      	 			while(packetpeker -> neste != NULL){
				      	 				packetpeker = packetpeker -> neste;
				      	 			}
				      	 			packetpeker->neste = pake;  	 			
				      	 	}
}
int lag_bilde(FILE* bilde, struct Image* en){
	char* image;

			fseek(bilde, 0, SEEK_END);
			long fsize = ftell(bilde);
			fseek(bilde, 0, SEEK_SET);   
			image = malloc(sizeof(char)*(fsize + 1));
			fread(image, sizeof(char), fsize, bilde);
			
			image[fsize] = '\0';

			struct Image* to = Image_create(image);
		
    		int svar = Image_compare(en,to);
    		Image_free(to);
    		free(image);
    		return svar;
}
void sammenlign(char* bildet , char* filnavn, const char* mappe, FILE* avsluttfil ){

	struct Image* en = Image_create(bildet);

	char path[100];

	//lag image vi har fått fra klienten
	
 	struct dirent *entry;  // Pointer for directory entry 
 	// opendir() returns a pointer of DIR type.  
    DIR *dr = opendir(mappe); 
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    { 
        perror("Could not open current directory" ); 
        exit(EXIT_FAILURE); 
    } 
    
        
    int lik=0;
    while((entry = readdir(dr)) != NULL){
    	//se om det er fil eller directory
    	if(entry->d_type == DT_REG){  // ser om det er en regular file
 
    		//lage path
    		strncpy(path,mappe, strlen(mappe));
    		strncat(path, "/", 1 );
    		strncat(path,entry->d_name,strlen(entry->d_name));	
			FILE *bilde = fopen(path, "rb");

			int svar =  lag_bilde(bilde, en); //lager bilde to og sammenligner

    		if(svar == 1){ //bildene er identiske
    			lik=1;
    			fclose(bilde);	
    			
    			break;
    		}	
   			memset(path, 0, strlen(path));
    		fclose(bilde);      		
    		
    	}
    }   

   	Image_free(en);
	//SKRIV TIL FIL
	char *like = malloc(sizeof(char)*100);
	strncat(like, " <", 2 );
	strncpy (like, filnavn+ 8, strlen(filnavn)-7); 
	like[strlen(like)]= '\0';
	char *like2 = malloc(sizeof(char)*30);
	strcpy (like2, entry->d_name); 
	strncat(like, "> <", 3 );
	strncat(like, like2, strlen(like)+strlen(like2));
	strncat(like, ">\n", 3 );

	char *ulike = malloc(sizeof(char)*50);
	strncat(ulike, " <", 2 );
	strncpy (ulike, filnavn+ 8, strlen(filnavn)-7); 
	char* ulike2= "UNKNOWN";
	strncat(ulike, "> <", 3 );
	strncat(ulike, ulike2, strlen(ulike)+strlen(ulike2));
	strncat(ulike, ">\n", 3 );

    if(lik == 0){
    	fwrite(ulike , sizeof(char),sizeof(ulike), avsluttfil);
    }
    else{
    	fwrite(like , sizeof(char),strlen(like), avsluttfil);
    }
    free(like);free(like2);free(ulike);

}
struct Packet* lage_struct(char* buffer){
	 	//Lage struct av det vi får og printe det ut
			    struct Packet* pake = malloc(sizeof(struct Packet)); 
			      	 	//header
	      	 	int i=0;
	      	 	memcpy(&pake->totlengde, buffer, sizeof(int));
	      	 	i=i+sizeof(int);
	      	 	memcpy(&pake->sekvensnr, buffer+i, sizeof(unsigned char) );
	      	 	i=i+sizeof(unsigned char);
	      	 	memcpy(&pake->sekack, buffer+i, sizeof( unsigned char));
	      	 	i=i+sizeof(unsigned char);
	      	 	memcpy(&pake->flagg, buffer+i,sizeof(unsigned char));
	      	 	i=i+sizeof(unsigned char);
	      	 	memcpy(&pake->unused, buffer+i,sizeof(unsigned char));
	      	 	i=i+sizeof(unsigned char);
	      	 	//payload
	      	 	memcpy(&pake->define, buffer+i,sizeof(int));
	      	 	i=i+sizeof(int);
	      	 	memcpy(&pake->lengde_filnavn, buffer+i,sizeof(int));
	      	 	i=i+sizeof(int);
	      	 	memcpy(&pake->filnavn, buffer+i,pake->lengde_filnavn);
	      	 	i=i+(pake->lengde_filnavn);
	      	 	int bytes =(pake->totlengde)-i;
	      	 	
	      	 	pake->bilde = malloc(bytes+1);
	      	 	strcpy(pake->bilde, buffer+i);

	      	 	pake->neste = NULL;
	      	 	return pake;
	      	 	
}
void send_ack(const struct sockaddr* addr, socklen_t addrlen,int sockfd, struct Packet* pake){
		//LAGE ACK STRUCT
			   		struct Packet *ack = malloc(sizeof(struct Packet));
			   		char ackpack[8];
			   		ack->unused =  0x7f;
		      	 	ack->flagg= 0x2; //samme som 00000010 siden den inneholder ikke payload siden den er ack
		      	 	ack->sekack = pake->sekvensnr;	
		      	 	ack->sekvensnr = pake->sekvensnr;
		      	 	ack->totlengde = sizeof(ack->sekack)+sizeof(ack->flagg)+sizeof(ack->sekvensnr)+sizeof(int)+sizeof(ack->unused);      	 
		      	 	//SETTE INN I EN CHAR ; SEND
		      	 	 int j=0;
			      	 	memcpy(ackpack+j, &ack->totlengde, sizeof(int));
			      	 	j=j+sizeof(int);
			      	 	memcpy(ackpack+j, &ack->sekvensnr,  sizeof(unsigned char) );
			      	 	j=j+sizeof(unsigned char);
			      	 	memcpy(ackpack+j, &ack->sekack,  sizeof( unsigned char));
			      	 	j=j+sizeof(unsigned char);
			      	 	memcpy(ackpack+j, &ack->flagg, sizeof(unsigned char));
			      	 	j=j+sizeof(unsigned char);
			      	 	memcpy(ackpack+j, &ack->unused, sizeof(unsigned char));
			      	 	j=j+sizeof(unsigned char);
			      	 	
			      	 	 //send melding til client
			      	 	printf(" Sender ack for pakke  %d\n",teller );
					    sendto(sockfd, (const char *)ackpack, j,  
					        0,addr,addrlen) ; // er liten vits i å ha send_packet siden du ikke setter lossprobability  
				        free(ack); 

}
int main(int argc, char *argv[]) { 
    int sockfd; 
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr, cliaddr; 


	if(argc != 4)
	{ //finne ut om det er riktig programformat skrevet riktig i terminalen
		perror( "Programformatet er:  ./server port directory filnavn" );
		exit(EXIT_FAILURE);
	}
	char* a =argv[1];
	PORT = atoi(a);
   
	//ÅPNE FIL VI SKAL SKRIVE TIL
	FILE *avsluttfil= fopen(argv[3], "wb");

    // lager socket fildeskriptor og skjekekr for feil
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
    char mappe[10];
    strcpy(mappe, argv[2]);
      
    // Fyller inn info 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
      
    // Binder socketen to serveradressen og sjekke om det er feil
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    socklen_t  len; 
    int n; 
  
    len = sizeof(cliaddr);  //len is value/resuslt 
  		
      //GO BACK N + SELECT LOOP
    int loopStop =0;
    while(loopStop == 0){
    	
		    n = recvfrom(sockfd,( char*) buffer, MAXLINE,  
		                0, ( struct sockaddr *) &cliaddr, 
		                &len); 
		    if(n>8){ //dette betyr pakken er en vanlig pakke og ikke terminering

			    buffer[n] = '\0'; 
			    
			   struct Packet* pake = lage_struct(buffer);
	      	 	
      	 	
		   		if(pake->sekvensnr == teller){
					printf("MOTATT PAKKE FRA klient %d\n\n", pake->sekvensnr); 


					sammenlign(pake->bilde, pake->filnavn,mappe , avsluttfil);
											
						
					send_ack((const struct sockaddr *) &cliaddr, len,sockfd, pake);

					free(pake->bilde);
					free(pake);
					teller++;

		   		}
		   		else{
		   			
		   			// free it and leave it 

		   			printf(" Forventer sekvensnr %d, men fikk %d\n, Fjernes\n",teller,pake->sekvensnr );
		   			free(pake->bilde);
					free(pake);

		   		}
	   			
			}

			else if(n<0) {
				perror("feil har skjedd i recvfrom\n");
			}
			else{ //termineringspakke

				printf("termineringpakken KOM!\n" );
				fclose(avsluttfil);
				loopStop++;
			}
	}	
	   		
      printf("FERDIG\n");

   
    return 0; 
}





