#include "stdio.h"#include "stdlib.h"#include <iostream>#include <fstream>#include <string>#include <map>#include "cv.h"#include "highgui.h"#include "CvxText.h"//#include "conio.h"#include <unistd.h>#include <dirent.h>#include <sys/stat.h>using namespace std;using namespace cv;#define BATCHPROCESS 0#define  MAX_LINE_LENGTH 256////////////////////////////////////////////////////////////////////////////// load dictionary, get the map for index2str and str2indexint LoadDictionaryMap(const char* dicPath,map<int,string>& dicMapIndex2Str,map<string,int>& dicMapStr2index);int LoadDictionaryIndex(const char* indexDicPath,map<int,int>& dicMapIndex2Label);////////////////////////////////////////////////////////////////////////////// projection on binary image for refine the top and bottomvoid GetTopAndBottom(Mat& binImg,int &top,int& bottom);////////////////////////////////////////////////////////////////////////////// get all the character's bound box on binary imageint GetCharBoxes(Mat& binImg,int charNum,vector<Rect>& boxes,int& top,int& bottom);int main(int argc, char* argv[]){    string outputDir = "../../data/generatedSamples/xsz";	string dicPath = "dict.txt";	string dicIdxpath = "dict-index.txt";	int sampleNum = 10;	int maxStrLen = 10;	int minStrLen = 5;	int strLength = 5;	int boarderSize = 2;	bool IsKX = false;	bool isRandLen = true;	CvScalar fontColor(CV_RGB(0,0,0));	bool isRandColor = false;	string backgroundImgPath = "background.jpg";	string fontPath = "simkai.ttf";	bool randTrasparent = true;	float transparency = 0.5;	float sp = 0.05;    // 字符间隔空间相对于字符的比率	int charSize = 100; //字符大小	bool randCharSize = true;	for (int i = 0; i < argc; i ++)	{		if (strcmp(argv[i],"--IsKX")==0)		{			IsKX = true;		}		if (strcmp(argv[i],"--OutputPath")==0)		{			i ++;			outputDir = argv[i]; 		}		if (strcmp(argv[i],"--DicPath")==0)   // set the dictionary path		{			i ++;			dicPath = argv[i];		}		if (strcmp(argv[i],"--SampleNumber")==0) // generated sample number		{			i ++;			sampleNum = atoi(argv[i]);		}		if (strcmp(argv[i],"--StrLength")==0) // whether rand to select the length of string 		{			isRandLen = false;			i ++;			strLength = atoi(argv[i]);		}		if (strcmp(argv[i],"--RandColor")==0)  // whether rand to set the character color 		{			isRandColor = true;		}		if (strcmp(argv[i],"--R")==0)		{			i ++;			fontColor.val[0] = atoi(argv[i]);		}		if (strcmp(argv[i],"--G")==0)		{			i ++;			fontColor.val[1] = atoi(argv[i]);		}		if (strcmp(argv[i],"--B")==0)		{			i ++;			fontColor.val[2] = atoi(argv[i]);		}		if (strcmp(argv[i],"--BackgroudImgPath")==0) // set the background image path		{			i ++;			backgroundImgPath = argv[i];		}		if (strcmp(argv[i],"--FontPath")==0) // set the font file path		{			i ++;			fontPath = argv[i];		}		if (strcmp(argv[i],"--Transparency")==0) // set the transparency		{			i ++;			randTrasparent = false;			transparency = atof(argv[i]);		}		if (strcmp(argv[i],"--SeprateSpace")==0) // set the separator space ratio		{			i ++;			sp = atof(argv[i]);		}		if (strcmp(argv[i],"--CharSize")==0) // set the character size		{			i ++;			randCharSize = false;			charSize = atoi(argv[i]);		}	}	if(access(outputDir.c_str(), F_OK)==0||mkdir(outputDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==0)	{		printf("output directory is created or existed!\n");	}	else	{		printf("failed to create output path!%s\n",outputDir.c_str());	}	map<string,int> dicMapStr2Index;	map<int,string> dicMapIndex2Str;	map<int,int> dicMapIndex2Label;	int dicCharNum = LoadDictionaryMap(dicPath.c_str(),dicMapIndex2Str,dicMapStr2Index);	int dicIndexNum = LoadDictionaryIndex(dicIdxpath.c_str(),dicMapIndex2Label);	cout<<"total character number:"<<dicCharNum<<endl;	if(dicCharNum == 0)	{	   cout<<"failed to load dictionary!"<<endl;	   return -1;	}	 	srand(time(NULL));	CvxText text(fontPath.c_str());	Mat orgImg = imread(backgroundImgPath.c_str(), 1);	if (orgImg.empty())	{		cout<<"Error!Failed to open background image "<<backgroundImgPath<<endl;		orgImg.create(cvSize(720,576),CV_8UC3);		memset(orgImg.data,0,orgImg.rows*orgImg.cols*orgImg.channels());	}	for (int i = 0; i < sampleNum; i ++)	{		Mat img;		orgImg.copyTo(img);		if (isRandLen)		{			strLength = rand()%(maxStrLen-minStrLen+1);						strLength = strLength + minStrLen;		}		string randStr = "";		vector<int> vecIndex;		for (int j = 0; j < strLength; j ++)		{			int idx = rand()%dicCharNum;			randStr += dicMapIndex2Str[idx];			vecIndex.push_back(idx);		}				if (isRandColor)		{			fontColor.val[0] = rand()%256;			fontColor.val[1] = rand()%256;			fontColor.val[2] = rand()%256;		}						if (randCharSize)		{			charSize = rand()%100;			charSize = charSize>24?charSize:(charSize+24);		}				int posX = rand()%orgImg.cols;			int strWidth = charSize*strLength*(1+sp);		if(strWidth>orgImg.cols*0.9)		{			i --;			continue;		}		cout<<strLength<<":"<<randStr<<endl;		posX = posX+strWidth<orgImg.cols?posX:orgImg.cols-strWidth;	//	cout<<"posX:"<<posX<<" strWidth:"<<strWidth<<"imgWidth:"<<orgImg.cols<<endl;		int posY = rand()%orgImg.rows;		posY = posY>charSize?posY:(posY+charSize);		posY = (posY+charSize)<orgImg.rows?posY:(orgImg.rows-charSize-1);		float p;		if (randTrasparent)		{			p = float(rand()%100)/100;			p = p > 0.5 ? p : (p+0.5);		}		else		{			p = transparency;		}				CvScalar size;		size.val[0] = charSize;       		size.val[1] = 0.5;    		size.val[2] = sp;    		size.val[3] = 0;       		text.setFont(NULL, &size, NULL, &p);    		CvPoint point;		point.x = posX;		point.y = posY;				text.putText(img,randStr.c_str(), point, fontColor);			Mat strImg;		Rect r;		r.width = (point.x-posX-1)/*+charSize*0.9*/;		r.height = charSize;	//	cout<<"x:"<<posX<<"strwidth:"<<strWidth<<" last:"<<point.x<<"y:"<<posY+charSize<<endl;		r.x = posX;		r.y = posY - charSize*0.95;				char name[100] = {0};		sprintf(name,"%s/%d.jpg",outputDir.c_str(),i);		img(r).copyTo(strImg);		bool flag = imwrite(name,strImg);	 			Mat blankImg;		orgImg.copyTo(blankImg);		memset(blankImg.data,0,blankImg.cols*blankImg.rows*blankImg.channels());				CvPoint newPoint;		newPoint.x = posX;		newPoint.y = posY;		CvScalar newColor(CV_RGB(255,255,255));		p = 1.0;		text.setFont(NULL, &size, NULL, &p);		text.putText(blankImg,randStr.c_str(), newPoint, newColor);				blankImg(r).copyTo(strImg);			Mat binImg;		strImg.copyTo(binImg);		cvtColor(strImg,binImg,CV_BGR2GRAY);		for (int m = 0; m < binImg.cols*binImg.rows; m ++)		{			binImg.data[m] = binImg.data[m] > 128?255:0;		}	//	imwrite("bin.jpg",binImg);				vector<Rect> charBoxes;		int top,bottom;		int boxNum = GetCharBoxes(binImg,strLength,charBoxes,top,bottom);		if (boxNum != strLength)		{			cout<<"Error! Find boxes:"<<boxNum<<" rand character number:"<<strLength<<endl;			i --;			continue;		}	/*	for (int k = 0; k < boxNum; k ++)		{			cv::rectangle(strImg,charBoxes[k],CV_RGB(255,0,255));		}	*/		char resName[100] = {0};		sprintf(resName,"%s/%d.txt",outputDir.c_str(),i);		FILE* fp = fopen(resName,"w");		fprintf(fp,"%d-%d,%d\r\n",strLength,top,bottom);		for (int k = 0; k < strLength; k ++)		{			int index;			if (dicIndexNum != dicCharNum)			{				index = vecIndex[k];			}			else			{				index = dicMapIndex2Label[vecIndex[k]];			}						fprintf(fp,"%d-%d:%d:%d:%d;",index,charBoxes[k].x,charBoxes[k].y,charBoxes[k].width,charBoxes[k].height);		}		fprintf(fp,"\n%s\n",randStr.c_str());		fclose(fp);			}    return 0;}Mat AddBoard(Mat& binImg,int boardSize){	int width = binImg.cols;	int height = binImg.rows;	int newWidth = width + 2*boardSize;	int newHeight = height + 2*boardSize;	Mat newImg;	newImg.create(cvSize(newWidth,newHeight),CV_8UC1);	memset(newImg.data,0,newWidth*newHeight);	for (int i = 0; i < height; i ++)	{		for (int j = 0; j < width; j ++)		{			if (binImg.data[i*width+j]==0)			{			}		}	}	return newImg;}int LoadDictionaryMap(const char* dicPath,map<int,string>& dicMapIndex2Str,map<string,int>& dicMapStr2index){	dicMapIndex2Str.clear();	dicMapStr2index.clear();	ifstream dicIfs(dicPath);	if (!dicIfs.is_open())	{		return 0;	}	char dic[MAX_LINE_LENGTH] = {0};	int count = 0;	while(dicIfs.getline(dic,MAX_LINE_LENGTH))	{		for (int i = 1; i < 6; i ++)		{			if (dic[i] == '\r') // remove line feed			{				dic[i] = '\0';			 				break;			}		}		string ch(dic);				dicMapIndex2Str.insert(make_pair(count,ch));		dicMapStr2index.insert(make_pair(ch,count));		memset(dic,0,MAX_LINE_LENGTH);		count ++;	}	dicIfs.close();	return count;}int LoadDictionaryIndex(const char* indexDicPath,map<int,int>& dicMapIndex2Label){	dicMapIndex2Label.clear();	ifstream dicIfs(indexDicPath);	if (!dicIfs.is_open())	{		return 0;	}	char idxStr[MAX_LINE_LENGTH] = {0};	int count = 0;	while(dicIfs.getline(idxStr,MAX_LINE_LENGTH))	{		int idx = atoi(idxStr);		dicMapIndex2Label.insert(make_pair(count,idx));		count ++;	}	dicIfs.close();	return count;}void GetTopAndBottom(Mat& binImg,int &top,int& bottom){	int width = binImg.cols;	int height = binImg.rows;	for (int i = 0; i < height; i ++)	{		for (int j= 0; j < width; j ++)		{			if (binImg.data[i*width+j] == 255)			{				top = i;				i = height;				break;			}		}	}	for (int i = height-1; i >= 0; i --)	{		for (int j = 0; j < width; j ++)		{			if (binImg.data[i*width+j] == 255)			{				bottom = i;				i = -1;				break;			}		}	}}int GetCharBoxes(Mat& binImg,int charNum,vector<Rect>& boxes,int& top,int& bottom){	boxes.clear();	GetTopAndBottom(binImg,top,bottom);	cout<<"top:"<<top<<"bottom:"<<bottom<<endl;	int width = binImg.cols;	int height = binImg.rows;	int* hist = new int[width];	memset(hist,0,width*sizeof(int));	for (int i = 0; i < width; i ++)	{		for (int j = 0; j < height; j ++)		{			if (binImg.data[j*width+i] == 255)			{				hist[i] ++;			}		}	}	int maxCharWidth = width/charNum;	//	cout<<"maxCharWidth:"<<maxCharWidth<<endl;	bool flag = true;	int beginX,endX;	if (hist[0]!=0)	{		beginX = 0;		flag = false;	}	for (int i = 1; i < width; i ++)	{		if (flag)		{			if (hist[i]!=0&&hist[i-1]==0)			{				beginX = i;				flag = false;			}		}		else		{			if (hist[i]==0&&hist[i-1]!=0)			{				int tmpCharWidth = i - beginX;				if (tmpCharWidth>maxCharWidth*0.6&&tmpCharWidth<maxCharWidth*1.1)				{					endX = i;					Rect rec;					rec.x = beginX;					rec.y = 0;//top;					rec.width = endX - beginX + 1;					rec.height = height;//bottom - top + 1;					int t,b;					Mat charImg;					binImg(rec).copyTo(charImg);					GetTopAndBottom(charImg,t,b);					int h = b - t + 1;					if (h > height*0.7)					{						rec.y = t;						rec.height= h;					}					else					{						rec.y = top;						rec.height = bottom - top + 1;					}					boxes.push_back(rec);					flag = true;				}			}		}	}	delete[] hist;	return boxes.size();}