//11 August 2016 18:30
//27 August 2016 morning
//20 Marzo 2017: - Se agregó la validación de mensajes
//				 - Lectura de imágenes y recorte de subimágenes


#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
//#include <dir.h> 
#include <netinet/in.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string>
#include <math.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sstream>
#include <streambuf>
//#include <fstream>

#include <ctime>

#include "lstStructs.h"

#define NUM_THREADS     5

void *sender(void *arg);
void error(const char *msg);
void obtainIP(char* host);
bool sendBigFrame( int newsockfd, std::string bigFrame );
bool sendFile( int newsockfd, std::ifstream &infile );
void funcPrintFirst(int n, int max, char *buffer);
bool getRaspImg(strReqImg *reqImg, const std::string& fileName);
unsigned char *funcCropImg(unsigned char* original, int origW, int x1, int y1, int x2, int y2);
std::string file2String( const std::string &fileName );
bool funcSaveFile(char* fileName, char* data, int fileLen, int w, int h);
std::string *genSLIDECommand(strReqImg *reqImg);
std::string *genCommand(strReqImg *reqImg, const std::string& fileName);
bool reqImgIsValid( strReqImg *reqImg );
bool fileExists( const std::string& fileName );
bool applyTimeLapseUsingRaspistill(strReqImg *reqImg);
//bool funcFolderExists( std::string folderPath );
bool funcPathExists( std::string fileName );
bool funcCreateFolder( std::string folderName );
bool funcDeleteFolderRecursively( std::string folderName );
bool funcClearFolder( std::string folderName );



const unsigned int PORT  = 51717;
//const unsigned int outPORT = 51717;//Mas grande

int main(int argc, char *argv[])
{
  pthread_t threadReceiver, threadSender;
  int rcReceiver, rcSender;
  int i = 0;
 
  //Define variables
  strReqImg *reqImg 	= (strReqImg*)malloc(sizeof(strReqImg));
  //strReqImg *reqImgSqu 	= (strReqImg*)malloc(sizeof(strReqImg));
  
  //Obtain the IP address
  char* host = (char*)malloc(NI_MAXHOST * sizeof(char));
  obtainIP(host);

  //Obtains the camera's name
  char camName[] = "IRis\0";
  FILE* pipe;
  std::string result;

  //Buffer
  char bufferComm[streamLen];
  frameStruct *tmpFrame 		          = (frameStruct*)malloc(sizeof(frameStruct));
  frameStruct *frame2Send 		          = (frameStruct*)malloc(sizeof(frameStruct));
  frameStruct *frameReceived 	          = (frameStruct*)malloc(sizeof(frameStruct));
  structRaspcamSettings *raspcamSettings  = (structRaspcamSettings*)malloc(sizeof(structRaspcamSettings));
  structCamSelected *camSelected          = (structCamSelected*)malloc(sizeof(structCamSelected));
  unsigned int tmpFrameLen, headerLen;
  headerLen = sizeof(frameHeader);
  std::string auxFileName;
  

  camSelected->On = false;


  unsigned int tmpTamArch;
  float tmpNumMsgs;

  unsigned int fileLen;
  const unsigned int dataLen = 2592 * 1944 * 3;
  //unsigned char *data = new unsigned char[ dataLen ];
  std::ifstream infile;

  //int sockfd, newsockfd, portno;
  int sockfd, newsockfd;
  socklen_t clilen;
  char buffer[frameBodyLen];
  
  struct sockaddr_in serv_addr, cli_addr;
  int n, aux;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    error("ERROR opening socket");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  //portno = inPORT;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    error("ERROR on binding");
  }
  
  
  
  
  
  
  
  
  
  
  
  /*
  printf("Executing raspistill\n");
  std::string fileName = "./tmpSnapshots/test.RGB888";
  if( getRaspImg(reqImg,fileName) ){
	  printf("Success\n");
	  std::string tmpImg = file2String(fileName);
	  printf("size: %d\n",tmpImg.size());
  }else{
	  printf("Fail\n");
  }
  return 1;
  */
  
  
  
  
  
  
  
  
  
  
  
  

  
  
  
  

  unsigned int holdon = 1;
  while( holdon ){
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd	 = accept(sockfd, 
               (struct sockaddr *) &cli_addr, 
               &clilen);
    if (newsockfd < 0){
      error("ERROR on accept");
    }
    bzero(buffer,frameBodyLen);
    n = read(newsockfd,buffer,frameBodyLen-1);
    if (n < 0){
      error("ERROR reading from socket");
    }

    //Ordering frame received
    aux = n-headerLen;       
    memcpy(frameReceived,buffer,n);

    //Extract the message and execute instruction identified
    printf("idMessage(%i) n(%i)\n",frameReceived->header.idMsg,n);
    //printf("Message(%c%c)\n",frameReceived->msg[0],frameReceived->msg[1]);
    printf("\n");
    switch( frameReceived->header.idMsg ){
      //Sending cam settings
      case 1:
        printf("Hand-shaking\n");
        //Send ACK
        camSettings tmpCamSettings;
        tmpCamSettings.idMsg = (char)1;
        memcpy(&tmpCamSettings.IP,host,15);        
        memcpy(&tmpCamSettings.Alias,camName,20);
        n = write(newsockfd,&tmpCamSettings,sizeof(camSettings));
        if (n < 0){
          error("ERROR writing to socket");
        }
        //printf("n(%i)\n", n);
        break;
      //Execute command and send result
      case 2:
        printf("Applying command -> %s\n",frameReceived->msg);

        //Get command result
        result = "";//idMsg to send
        pipe = popen(frameReceived->msg, "r");
        try {
          while (!feof(pipe)) {
            if (fgets(bufferComm, frameBodyLen, pipe) != NULL){
              result.append( bufferComm );
            }
          }
        } catch (...) {
          pclose(pipe);
          throw;
        }
        pclose(pipe);


        //Send message
        result = (result.size()>1)?result:"Empty result";
        printf("\n<%s>\n", result.c_str());
        if( sendBigFrame( newsockfd, result ) ){
          printf("Command result sent\n");
        }else{
          printf("Command result NOT sent\n");
        }
        

        /*
        //Prepare message
        frame2Send->header.idMsg        = (char)2;  
        frame2Send->header.consecutive  = 1;        
        frame2Send->header.numTotMsg    = 1;        
        frame2Send->header.bodyLen      = result.size();
        bzero(frame2Send->msg,frameBodyLen);
        std::copy(result.begin(), result.end(), frame2Send->msg);
        tmpFrameLen = sizeof(frameHeader)+frame2Send->header.bodyLen;
        n = write(newsockfd,frame2Send,tmpFrameLen);        
        if (n < 0){
          error("ERROR writing to socket");
        }
        */
        break;
      //Execute command and omit result (only done ack)
      case 3:

        //Aply message
        printf("Applying command -> %s\n",frameReceived->msg);
        pipe = popen(frameReceived->msg, "r");
        result.assign("done");

        //Send result
        printf("\n<%s>\n", result.c_str());
        if( sendBigFrame( newsockfd, result ) ){
          printf("Command DONE sent\n");
        }else{
          printf("Command DONE NOT sent\n");
        }

        //Prepare message
        /*
        frame2Send->header.idMsg        = (char)2;  
  	    frame2Send->header.consecutive  = 1;        
  	    frame2Send->header.numTotMsg    = 1;        
  	    frame2Send->header.bodyLen      = result.size();
  	    bzero(frame2Send->msg,frameBodyLen);
  	    std::copy(result.begin(), result.end(), frame2Send->msg);
        tmpFrameLen = sizeof(frameHeader)+frame2Send->header.bodyLen;
        n = write(newsockfd,frame2Send,tmpFrameLen);        
        if (n < 0){
          error("ERROR writing to socket");
        }
        */
        break;

    case 4:
		printf("Image requested\n");
		infile.open("yo.jpg", std::ifstream::binary);
		if( sendFile( newsockfd, infile ) ){
			printf("File sent\n");
		}else{
			printf("File NOT sent\n");
		}
		infile.close();
		break;
    
    //Camera operations      
    case 5:
      break;

    //Send image from camera
    case 6:
      break;

    //Require image from scratch
    //..
    case 7:
		//Prepare memory
		memset( reqImg, '\0', sizeof(strReqImg)  );		
		memcpy( reqImg, frameReceived, sizeof(strReqImg) );
		//Send ACK with camera status
		if( reqImgIsValid( reqImg ) )
			buffer[1] = 1;
		else
			buffer[1] = 0;	
		write(newsockfd,&buffer,2);
		
		if( buffer[1] == 0 )break;
		else{ 
			//Get image
			//..
			printf("Making the snapshot by applying raspistill\n");
			std::string fileName = "./tmpSnapshots/tmpImg.RGB888";
			if( getRaspImg(reqImg,fileName) ){
			  printf("Snapshot [OK]\n");
			}else{
			  printf("Snapshot[Fail]\n");
			  break;
			}

			//Send image as frame
			//..
			std::string tmpImg = file2String(fileName);
			printf("size: %d\n",tmpImg.size());
			if( sendBigFrame( newsockfd, tmpImg ) ){
				printf("Photogram sent\n");
			}else{
				printf("ERROR sending photo\n");
			}		

		}
    	break;

	case 8:
		//Prepare memory		
		memset( reqImg, '\0', sizeof(strReqImg) );
		memcpy( reqImg, frameReceived, sizeof(strReqImg) );
		printf("Square: x(%d) y(%d) w(%d) h(%d)\n",reqImg->sqApSett.rectX,reqImg->sqApSett.rectY,reqImg->sqApSett.rectW,reqImg->sqApSett.rectH);

		//Send ACK with camera status
		buffer[1] = 1;
		write(newsockfd,&buffer,2);
		if( buffer[1] == 0 )break;
		else{
			/* 
			//Get diffracted image
			//..			
			printf("Making the snapshot by applying raspistill\n");
			std::string fileNameDiff = "./tmpSnapshots/tmpImgDiff.RGB888";
			reqImg->squApert = false;
			reqImg->needCut	 = true;
			if( getRaspImg(reqImg,fileNameDiff) ){
			  printf("Snapshot diffraction [OK]\n");
			}else{
			  printf("Snapshot diffraction [Fail]\n");
			  break;
			}
			*/
			
			
			//Get square-aperture image
			//..
			printf("Making the square-aperture snapshot by applying raspistill\n");
			std::string fileNameSqu = "./tmpSnapshots/tmpImgSqu.RGB888";
			reqImg->squApert = true;
			reqImg->needCut	 = true;
			if( getRaspImg(reqImg,fileNameSqu) ){
			  printf("Snapshot square-aperture [OK]\n");
			}else{
			  printf("Snapshot square-aperture [Fail]\n");
			  break;
			}		
		}
    	break;

	//
    //Require slide-cube from scratch
    //..
    case 9:
		//Prepare memory
		memset( reqImg, '\0', sizeof(strReqImg)  );		
		memcpy( reqImg, frameReceived, sizeof(strReqImg) );
		
		//Send ACK with camera status
		if( reqImgIsValid( reqImg ) )
			buffer[1] = 1;
		else
			buffer[1] = 0;	
		write(newsockfd,&buffer,2);
		
		//Wait for instruction to start time lapse
		n = read(newsockfd, buffer, frameBodyLen-1);
		if (n < 0)
		{
			error("Sock: Before start time lapse");
		}
	
		//Start time lapse and send ACK at the end of the proccess
		if( applyTimeLapseUsingRaspistill( reqImg ) == true )
			buffer[1] = 1;
		else 
			buffer[1] = 0;
		write(newsockfd,&buffer,2);
		
		//funcClearFolder( "borrarCarpeta" );
		
		
    	break;

    //Unrecognized instruction
    default:
      n = write(newsockfd,"Default",8);
      if (n < 0)
      {
        error("ERROR writing to socket");
      }
      holdon = 0;
      break;

    }

    
  }

  close(newsockfd);
  close(sockfd);
  printf("It finishes...\n");
  return 0;
}

bool applyTimeLapseUsingRaspistill(strReqImg *reqImg)
{
	std::string *timeLapseCommand = genSLIDECommand(reqImg);
	printf("Comm: %s\n",timeLapseCommand->c_str());
			
	/*
	//Execute raspistill
	FILE* pipe;
	pipe = popen(timeLapseCommand->c_str(), "r");
	pclose(pipe);
	*/	
	
	return true;
}


std::string file2String( const std::string &fileName ){
	std::ifstream t(fileName.c_str());
	std::string str;
	t.seekg(0,std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0,std::ios::beg);
	str.assign(
					(std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>()
			  );
	return str;
}

bool funcSaveFile(char* fileName, char* data, int fileLen, int w, int h){
	std::ofstream outFile( fileName,std::ios::binary );
	outFile<<"P6\n"<<w<<" "<<h<<" 255\n";
	outFile.write ( data, fileLen  );
	return true;
}

unsigned char *funcCropImg(unsigned char* original, int origW, int x1, int y1, int x2, int y2){
	//Crop image
	int lastW = x2-x1;
	int lastH = y2-y1;
	unsigned char *lastImg = new unsigned char[(lastW*lastH*3)];
	int r, aux;
	for( r=0; r<lastH; r++ ){
		aux = ((y1-1+r)*origW*3)+(x1*3);
		memcpy( &lastImg[r*lastW*3], &original[aux],(lastW*3));
	}

	//save
	//..
	printf("lastW(%d) lastH(%d)\n",lastW,lastH);
	char tmpFileName[] = "./tmpSnapshots/crop.ppm";
	funcSaveFile(tmpFileName,(char*)lastImg, (lastW*lastH*3), lastW, lastH);
	return lastImg;
}


bool getRaspImg(strReqImg *reqImg, const std::string &fileName){
	
	//Concatenate raspistill command
	//raspistill -q 100 -gc -ifx colourbalance -ifx denoise  -o test.RGB888 -t 8000 -ss 1000000 -roi x,y,w,d 
	//..
	std::string *raspistillCommand = genCommand(reqImg, fileName);
	printf("Comm: %s\n",raspistillCommand->c_str());
	//Prepare command as required
	//char *tmpComm = new char[raspistillCommand->size()+1];
	//std::copy(raspistillCommand->begin(), raspistillCommand->end(),tmpComm);
	//tmpComm[raspistillCommand->size()] = '\0';
	
	//Execute raspistill
	//..
	//Remove file if exists	
	if(fileExists(fileName)){
		remove(fileName.c_str());	
	}
	//Execute raspistill
	FILE* pipe;
	//pipe = popen(tmpComm, "r");
	pipe = popen(raspistillCommand->c_str(), "r");
	pclose(pipe);
	//delete[] tmpComm;
	
	//Verify if it was created the snapshot
	//..
	if(!fileExists(fileName)){
		return false;
	}
	
	
	return true;
		
}

bool reqImgIsValid( strReqImg *reqImg )
{
	//Validates image required size
	if( reqImg->imgCols <= 0 )
	{
		printf("Width <= 0\n");
		return false;
	}	
	if( reqImg->imgRows <= 0 )
	{
		printf("Height <= 0\n");
		return false;
	}	
	
	//Validates area of interes
	if( reqImg->needCut == true )
	{
		printf("Cut required\n");
		
		if( reqImg->sqApSett.rectW <= 0 )
		{
			printf( "ROI W <= 0\n" );
			return false;
		}
		if( reqImg->sqApSett.rectH <= 0 )
		{
			printf( "ROI H <= 0\n" );
			return false;
		}
		if( reqImg->sqApSett.rectW < 0 )
		{
			printf( "ROI X < 0\n" );
			return false;
		}
		if( reqImg->sqApSett.rectW < 0 )
		{
			printf( "ROI Y < 0\n" );
			return false;
		}
	}
	
	//
	if( reqImg->isSlide == true )
	{
		if( reqImg->slide.x1 < 0 ){printf( "x1 < 0\n" );return false;}
		if( reqImg->slide.y1 < 0 ){printf( "y1 < 0\n" );return false;}
		if( reqImg->slide.rows1 < 1 ){printf( "rows1 < 1\n" );return false;}
		if( reqImg->slide.cols1 < 1 ){printf( "cols1 < 1\n" );return false;}
		
		if( reqImg->slide.x2 < 0 ){printf( "x2 < 0\n" );return false;}
		if( reqImg->slide.y2 < 0 ){printf( "y2 < 0\n" );return false;}
		if( reqImg->slide.rows2 < 1 ){printf( "rows2 < 1\n" );return false;}
		if( reqImg->slide.cols2 < 1 ){printf( "cols2 < 1\n" );return false;}
		
		if( reqImg->slide.speed < 100 ){printf( "speed < 100\n" );return false;}
		if( reqImg->slide.duration < 500 ){printf( "duration < 500\n" );return false;}
	}
		
	
	
	
	
	return true;
}

std::string *genSLIDECommand(strReqImg *reqImg)
{
	//
	//Initialize command
	//..
	std::string *tmpCommand = new std::string("raspistill -o ./timeLapseAutomatic/%d.png"); //5000 -tl 200");
	std::ostringstream numberToString;
	tmpCommand->append(" -n -q 100 -gc");
	
	//Add lapse (speed) and duration
	numberToString.str("");
	numberToString << " -t " << reqImg->slide.duration << " -tl " << reqImg->slide.speed;
	tmpCommand->append( numberToString.str() );

	
	
	
	/*
	//Colour balance?
	if(reqImg->raspSett.ColorBalance){
		tmpCommand->append(" -ifx colourbalance");
	}
	//Denoise?
	if(reqImg->raspSett.Denoise){
		tmpCommand->append(" -ifx denoise");
	}	
	//Square Shuter speed
	int shutSpeed = reqImg->raspSett.SquareShutterSpeed + reqImg->raspSett.SquareShutterSpeedSmall;
	if( (reqImg->squApert && shutSpeed>0))
	{		
		ss.str("");
		ss<<shutSpeed;
		tmpCommand->append(" -ss " + ss.str());
	}
	*/
	
	
	return tmpCommand;
}

std::string *genCommand(strReqImg *reqImg, const std::string& fileName)
{
	
	//Initialize command
	//..
	std::string *tmpCommand = new std::string("raspistill -o ");
	std::ostringstream ss;
	tmpCommand->append(fileName);
	tmpCommand->append(" -n -q 100 -gc");
	//Colour balance?
	if(reqImg->raspSett.ColorBalance){
		tmpCommand->append(" -ifx colourbalance");
	}
	//Denoise?
	if(reqImg->raspSett.Denoise){
		tmpCommand->append(" -ifx denoise");
	}	
	//Square Shuter speed
	int shutSpeed = reqImg->raspSett.SquareShutterSpeed + reqImg->raspSett.SquareShutterSpeedSmall;
	if( (reqImg->squApert && shutSpeed>0))
	{		
		ss.str("");
		ss<<shutSpeed;
		tmpCommand->append(" -ss " + ss.str());
	}
	//Diffraction Shuter speed
	shutSpeed = reqImg->raspSett.ShutterSpeed + reqImg->raspSett.ShutterSpeedSmall;
	if(
		(!reqImg->squApert && shutSpeed>0) ||	//Whe is by parts
		(reqImg->fullFrame  && shutSpeed>0)	//Whe is unique and shutter speed has been setted
	)
	{
		ss.str("");
		ss<<shutSpeed;
		tmpCommand->append(" -ss " + ss.str());
	}
	//Trigering timer
	if( reqImg->raspSett.TriggerTime > 0 )
	{
		ss.str("");
		ss<<(reqImg->raspSett.TriggerTime*1000);
		tmpCommand->append(" -t " + ss.str());
	}
	else
	{
		ss.str("");
		ss<<250;//Milliseconds by default
		tmpCommand->append(" -t " + ss.str());
	}	
	
	//Crop image if neccesary
	//..
	if( reqImg->needCut )
	{
		float x,y,w,d;	
		//Normalization
		if(reqImg->squApert)
		{
			x = (float)reqImg->sqApSett.rectX / (float)_BIG_WIDTH;
			y = (float)reqImg->sqApSett.rectY / (float)_BIG_HEIGHT;
			w = (float)reqImg->sqApSett.rectW / (float)_BIG_WIDTH;
			d = (float)reqImg->sqApSett.rectH / (float)_BIG_HEIGHT;
			//printf("Square: x(%.4f) y(%.4f) w(%.4f) h(%.4f)\n",x,y,w,d);
			//printf("Square: x(%d) y(%d) w(%d) h(%d)\n",reqImg->sqApSett.rectX,reqImg->sqApSett.rectY,reqImg->sqApSett.rectW,reqImg->sqApSett.rectH);
		}
		else
		{
			x = (float)reqImg->diffArea.rectX / (float)_BIG_WIDTH;
			y = (float)reqImg->diffArea.rectY / (float)_BIG_HEIGHT;
			w = (float)reqImg->diffArea.rectW / (float)_BIG_WIDTH;
			d = (float)reqImg->diffArea.rectH / (float)_BIG_HEIGHT;
			printf("Diff: x(%.4f) y(%.4f) w(%.4f) h(%.4f)\n",x,y,w,d);
		}
		//Add region of interes to the raspistill command
		tmpCommand->append(" -roi ");		
		ss.str("");
		ss<<x;
		tmpCommand->append(ss.str()+",");
		ss.str("");
		ss<<y;
		tmpCommand->append(ss.str()+",");
		ss.str("");
		ss<<w;
		tmpCommand->append(ss.str()+","	);
		ss.str("");
		ss<<d;
		tmpCommand->append(ss.str());
		//Width and Height
		if(reqImg->squApert)
		{
			//Width
			ss.str("");
			ss<<reqImg->sqApSett.rectW;
			tmpCommand->append(" -w " + ss.str());
			//Height
			ss.str("");
			ss<<reqImg->sqApSett.rectH;
			tmpCommand->append(" -h " + ss.str());
		}
		else
		{
			//Width
			ss.str("");
			ss<<reqImg->diffArea.rectW;
			tmpCommand->append(" -w " + ss.str());
			//Height
			ss.str("");
			ss<<reqImg->diffArea.rectH;
			tmpCommand->append(" -h " + ss.str());
		}
	}
	else
	{
		//Width
		ss.str("");
		ss<<reqImg->imgCols;
		tmpCommand->append(" -w " + ss.str());
		//Height
		ss.str("");
		ss<<reqImg->imgRows;
		tmpCommand->append(" -h " + ss.str());
	}
	
	//AWB
	if(strcmp((char*)reqImg->raspSett.AWB, "none")!=0){
		std::string sAWB((char*)reqImg->raspSett.AWB, sizeof(reqImg->raspSett.AWB));
		tmpCommand->append(" -awb ");
		tmpCommand->append(sAWB.c_str());
		//printf("Entro a AWB: %s\n",sAWB.c_str());
	}
	
	//Exposure
	if(strcmp((char*)reqImg->raspSett.Exposure, "none")!=0){
		std::string sExposure((char*)reqImg->raspSett.Exposure, sizeof(reqImg->raspSett.Exposure));
		tmpCommand->append(" -ex ");
		tmpCommand->append(sExposure.c_str());
		//printf("Entro a Exp: %s\n",sExposure.c_str());
	}
	
	return tmpCommand;
}

bool fileExists( const std::string& fileName ){
	struct stat buffer;
	return( stat(fileName.c_str(), &buffer) == 0 );
}

bool sendBigFrame( int newsockfd, std::string bigFrame ){
  //printf("Inside sendBigFrame\n");

  //get file properties
  std::string result;
  frameStruct *frame2Send = (frameStruct *)malloc(sizeof(frameStruct));
  //infile.open("yo.jpg", std::ifstream::binary);
  //infile.seekg (0,infile.end);
  unsigned int fileLen;
  char buffer[frameBodyLen];
  int i, n, aux;

  //printf("Len sent\n");
  fileLen = bigFrame.size();
  //infile.seekg (0);
  if( fileLen < 1 ){
    //Prepare message
    result.assign("Error reading file");
    frame2Send->header.idMsg        = (char)3;  
    frame2Send->header.consecutive  = 1;
    frame2Send->header.bodyLen      = result.size();
    bzero(frame2Send->msg,frameBodyLen);        
    std::copy(result.begin(), result.end(), frame2Send->msg);
    unsigned int tmpFrameLen = sizeof(frameHeader)+frame2Send->header.bodyLen;
    n = write(newsockfd,frame2Send,tmpFrameLen);        
    if (n < 0){
      error("ERROR writing to socket");
    }
  }else{

    //Send Len
    //printf("fileLen: %i\n", fileLen);
    n = write(newsockfd,&fileLen,sizeof(unsigned int));
    if (n < 0){
      error("ERROR writing to socket");      
    }
    //printf("Len sent\n");

    //Get file request
    //printf("Reading\n");
    n = read(newsockfd, buffer, frameBodyLen-1);
    if (n < 0){
      error("ERROR reading from socket");
    }
    //printf("File requested\n");

    //Send file
    aux = floor( (float)fileLen / (float)frameBodyLen );
    aux = ((aux*frameBodyLen)<fileLen)?aux+1:aux;
    printf("numMsgs: %i - frameBodyLen: \n",aux,frameBodyLen);
    for(i=1; i<=aux; i++){
      //Send part of the file
      bzero(buffer,frameBodyLen);
      memcpy( buffer, &bigFrame[(i-1)*frameBodyLen], frameBodyLen );
      n = write(newsockfd,buffer,frameBodyLen);
      if (n < 0){
        error("ERROR writing to socket");
      }
      //Get next part requesst
      n = read(newsockfd, buffer, frameBodyLen-1);
      if (n < 0){
        error("ERROR reading from socket");
      }
    }
  }
  //infile.close();

  //printf("bigFrame finish\n");
  return true;
}


bool sendFile( int newsockfd, std::ifstream &infile ){
  //printf("Image requested inside\n");

  //get file properties
  std::string result;
  frameStruct *frame2Send = (frameStruct *)malloc(sizeof(frameStruct));
  //infile.open("yo.jpg", std::ifstream::binary);
  infile.seekg (0,infile.end);
  unsigned int fileLen;
  char buffer[frameBodyLen];
  int i, n, aux;

  //printf("Len sent\n");
  fileLen = infile.tellg();
  infile.seekg (0);
  if( fileLen < 1 ){
    //Prepare message
    result.assign("Error reading file");
    frame2Send->header.idMsg        = (char)3;  
    frame2Send->header.consecutive  = 1;
    frame2Send->header.bodyLen      = result.size();
    bzero(frame2Send->msg,frameBodyLen);        
    std::copy(result.begin(), result.end(), frame2Send->msg);
    unsigned int tmpFrameLen = sizeof(frameHeader)+frame2Send->header.bodyLen;
    n = write(newsockfd,frame2Send,tmpFrameLen);        
    if (n < 0){
      error("ERROR writing to socket");
    }
  }else{

    //Send Len
    n = write(newsockfd,&fileLen,sizeof(unsigned int));
    if (n < 0){
      error("ERROR writing to socket");
    }
    //printf("Len sent\n");

    //Get file request
    n = read(newsockfd, buffer, frameBodyLen-1);
    if (n < 0){
      error("ERROR reading from socket");
    }
    //printf("File requested\n");

    //Send file
    aux = floor( (float)fileLen / (float)frameBodyLen );
    for(i=1; i<=aux; i++){
      //Send part of the file
      bzero(buffer,frameBodyLen);
      infile.read(buffer,frameBodyLen);

      //funcPrintFirst(3,frameBodyLen,buffer);



      n = write(newsockfd,buffer,frameBodyLen);
      if (n < 0){
        error("ERROR writing to socket");
      }
      //printf("File part sent(%i)\n",i);

      //Get next part requesst
      n = read(newsockfd, buffer, frameBodyLen-1);
      if (n < 0){
        error("ERROR reading from socket");
      }

      //printf("File part Requested(%i)\n",i);
    }
    //Send last part of the file
    aux = fileLen - (aux*frameBodyLen);
    if( aux > 0 ){          
      bzero(buffer,frameBodyLen);
      infile.read(buffer,aux);
      n = write(newsockfd,buffer,aux);
      if (n < 0){
        error("ERROR writing to socket");
      }
      //printf("Last part sent i(%i) aux(%i) \n",i,aux);
    }else{
      //printf("Last part does not exist aux(%i) \n",aux);
    }
    //printf("Image sent\n");

  }
  //infile.close();

  return true;
}









void funcPrintFirst(int n, int max, char *buffer){
  int i;
  printf("First: ");
  for(i=0;i<n;i++){
    printf(" %i ", (int)buffer[i]);
  }
  printf("\nLast: ");
  for(i=max-1;i>=max-n;i--){
    printf(" %i ", (int)buffer[i]);
  }
  printf("\n\n");
}

void *sender(void *arg){
  printf("Sender\n");
  return NULL;
}
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
void obtainIP(char* host)
{
    FILE *f;
    char line[100] , *p , *c;
     
    f = fopen("/proc/net/route" , "r");
     
    while(fgets(line , 100 , f))
    {
        p = strtok(line , " \t");
        c = strtok(NULL , " \t");
         
        if(p!=NULL && c!=NULL)
        {
            if(strcmp(c , "00000000") == 0)
            {
                //printf("Default interface is : %s \n" , p);
                break;
            }
        }
    }
     
    //which family do we require , AF_INET or AF_INET6
    int fm = AF_INET;
    struct ifaddrs *ifaddr, *ifa;
    int family , s;
    //char host[NI_MAXHOST];
    
 
    if (getifaddrs(&ifaddr) == -1) 
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
 
    //Walk through linked list, maintaining head pointer so we can free list later
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }
 
        family = ifa->ifa_addr->sa_family;
 
        if(strcmp( ifa->ifa_name , p) == 0)
        {
            if (family == fm) 
            {
                s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);
                 
                if (s != 0) 
                {
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                    exit(EXIT_FAILURE);
                }
                 
                //printf("address: %s", host);
            }
            printf("\n");
        }
    }
 
    freeifaddrs(ifaddr);

}

bool funcPathExists( std::string fileName )
{
	FILE* fileAsked = fopen( fileName.c_str(), "r" );
	if( fileAsked )
	{
		fclose( fileAsked );
		return true;
	}	
	return false;
}

bool funcCreateFolder( std::string folderName )
{
	if( funcPathExists( folderName ) == true )
	{
		printf( "Cannot create folder (%s), folder exits\n", folderName.c_str() );		
		return false;
	}
	else
	{
		std::string *tmpCommand = new std::string("mkdir -m777 ");
		tmpCommand->append( folderName );		
		system( tmpCommand->c_str() );
	}
	return true;
}

bool funcDeleteFolderRecursively( std::string folderName )
{
	if( funcPathExists( folderName ) == false )
	{
		printf( "Cannot delete folder (%s), folder does not exits\n", folderName.c_str() );		
		return false;
	}
	else
	{
		std::string *tmpCommand = new std::string("rm -r ");
		tmpCommand->append( folderName );		
		system( tmpCommand->c_str() );
	}
	return true;
}

bool funcClearFolder( std::string folderName )
{
	if( funcPathExists( folderName ) == true )
		if( funcDeleteFolderRecursively( folderName ) == false )
			return false;
	return funcCreateFolder( folderName );	
}






