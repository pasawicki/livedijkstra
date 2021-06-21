#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/imgcodecs/imgcodecs.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/video/background_segm.hpp>
#include<opencv2/video.hpp>
#include<opencv2/videoio.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include<iostream>
#include<conio.h>
#include<windows.h>
#include<queue>

using namespace cv;
using namespace std;

int cost[650][650], prevy[650][650], prevx[650][650], visit[650][650];

void dijkstra(Mat pic, int x1, int y1, int x2, int y2, Mat drawTo, int xmax, int ymax)
{
	priority_queue < pair<int, pair<int, int>>> pq;
	for (int i = 0; i <= ymax; i++) for (int j = 0; j <= xmax; j++) {cost[i][j] = 999999999; visit[i][j] = 0;}
	cost[y1][x1] = 0;
	pq.push(make_pair(1000000000, make_pair(y1, x1)));
	cout << "DIJKSTRA!!!\n"; int xop = -1; int yop = -1;
	while ((visit[y2][x2] == 0) && (!pq.empty()))
	{
		pair<int, int> yopxop = pq.top().second;
		yop = yopxop.first; xop = yopxop.second;

		if (visit[yop][xop] == 0)
		{
		//licz++; cout << "O dziwo jeszcze działa... " << yop << " " << xop << " " << licz << "\n";

		if ((yop > 0) && (cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop - 1, xop)) + 1 < cost[yop - 1][xop]))
		{
			cost[yop - 1][xop] = cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop - 1, xop)) + 1;
			prevy[yop - 1][xop] = yop; prevx[yop - 1][xop] = xop;
			pq.push(make_pair(1000000000-cost[yop - 1][xop], make_pair(yop - 1, xop)));
		}
		if ((xop > 0) && (cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop, xop - 1)) + 1 < cost[yop][xop - 1]))
		{
			cost[yop][xop - 1] = cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop, xop - 1)) + 1;
			prevy[yop][xop - 1] = yop; prevx[yop][xop - 1] = xop;
			pq.push(make_pair(1000000000-cost[yop][xop - 1], make_pair(yop, xop - 1)));
		}
		if ((yop < ymax) && (cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop + 1, xop)) + 1 < cost[yop + 1][xop]))
		{
			cost[yop + 1][xop] = cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop + 1, xop)) + 1;
			prevy[yop + 1][xop] = yop; prevx[yop + 1][xop] = xop;
			pq.push(make_pair(1000000000-cost[yop + 1][xop], make_pair(yop + 1, xop)));
		}
		if ((xop < xmax) && (cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop, xop + 1)) + 1 < cost[yop][xop + 1]))
		{
			cost[yop][xop + 1] = cost[yop][xop] + abs(pic.at<uchar>(yop, xop) - pic.at<uchar>(yop, xop + 1)) + 1;
			prevy[yop][xop + 1] = yop; prevx[yop][xop + 1] = xop;
			pq.push(make_pair(1000000000-cost[yop][xop + 1], make_pair(yop, xop + 1)));
		}
		visit[yop][xop] = 1;
		}
		pq.pop(); //cout << cost[yop][xop] << " " << cost[y2][x2] << "\n";
	}

	int ycolor = y2; int xcolor = x2;
	if (visit[y2][x2]==1) while ((ycolor != y1) || (xcolor != x1))
	{
		Vec3b color = drawTo.at<Vec3b>(ycolor, xcolor);
		color[0] = 255; color[1] = 0; color[2] = 255;
		drawTo.at<Vec3b>(Point(xcolor, ycolor)) = color;
		int ynew = prevy[ycolor][xcolor]; int xnew = prevx[ycolor][xcolor];
		ycolor = ynew; xcolor = xnew;
	}

}

int main()
{
	Mat image, bwsrc, graysrc, pocz, prev, obraz1, obraz2, logo, dst, cdst, dst2, detected_edges, fgMask, fgImg, bgImg, old_image, old_gray, workPic;
	VideoCapture cap;
	int wykonaj, czas, klatki, licz, coun, prog, pom, pom2, mode, whatNow, pause, cease, endvid;
	double alfa;
	vector<int> compression_params;
	vector<Point> hull;
	vector<Point2f> corners0, corners1;
	String name, helpname;

	cap.open(0);
	//cap.open("C:/WidzKomp/bike.wm");
	//Jeżeli źródłem ma być wideoplik zamiast kamery, można (należy) zakomentować pierwszą linijkę i odkomentować drugą, zmieniając ją odpowiednio.

	if (!cap.isOpened()) //kontroler poprawności otwarcia
	{
		cout << "Obraz do przetwarzania nie moze byc udostepniony...\n";
		return(-1);
	}

	namedWindow("window", WINDOW_AUTOSIZE);
	namedWindow("trackbarWindow", 1);
	double dWidth = cap.get(CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CAP_PROP_FRAME_HEIGHT);

	int x1 = 100; createTrackbar("x1", "trackbarWindow", &x1, dWidth);
	int y1 = 200; createTrackbar("y1", "trackbarWindow", &y1, dHeight);
	int x2 = 100; createTrackbar("x2", "trackbarWindow", &x2, dWidth);
	int y2 = 300; createTrackbar("y2", "trackbarWindow", &y2, dHeight);
	int x3 = 200; createTrackbar("x3", "trackbarWindow", &x3, dWidth);
	int y3 = 350; createTrackbar("y3", "trackbarWindow", &y3, dHeight);
	int x4 = 300; createTrackbar("x4", "trackbarWindow", &x4, dWidth);
	int y4 = 300; createTrackbar("y4", "trackbarWindow", &y4, dHeight);
	int x5 = 300; createTrackbar("x5", "trackbarWindow", &x5, dWidth);
	int y5 = 200; createTrackbar("y5", "trackbarWindow", &y5, dHeight);
	int x6 = 200; createTrackbar("x6", "trackbarWindow", &x6, dWidth);
	int y6 = 150; createTrackbar("y6", "trackbarWindow", &y6, dHeight);

	coun = 0; prog = 25; alfa = 0.5; wykonaj = 0; mode = 1; pause = 0; cease = 0; endvid = 0; //reszta bez znaczenia, ale progiem detekcji czy parametrem alfa można manipulować...

	if (wykonaj==0) while (cap.read(image)) //strumień będący źródłem obrazu - wewnątrz warunku pętli!
	{
		coun++; //odkomentować ten długi ciąg linijek, aby otrzymać wersję z wykrywaniem ruchu...
	
		cvtColor(image, graysrc, cv::COLOR_RGB2GRAY);
		
		
		GaussianBlur(graysrc, graysrc, Size(9, 9), 3.5, 3.5);

		dijkstra(graysrc, x1, y1, x2, y2, image, dWidth - 1, dHeight - 1);
		dijkstra(graysrc, x2, y2, x3, y3, image, dWidth - 1, dHeight - 1);
		dijkstra(graysrc, x3, y3, x4, y4, image, dWidth - 1, dHeight - 1);
		dijkstra(graysrc, x4, y4, x5, y5, image, dWidth - 1, dHeight - 1);
		dijkstra(graysrc, x5, y5, x6, y6, image, dWidth - 1, dHeight - 1);
		dijkstra(graysrc, x6, y6, x1, y1, image, dWidth - 1, dHeight - 1);
		
		RNG rng(12345);
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));

		circle(image, Point2f(x1, y1), 3, color, FILLED);
		circle(image, Point2f(x2, y2), 3, color, FILLED);
		circle(image, Point2f(x3, y3), 3, color, FILLED);
		circle(image, Point2f(x4, y4), 3, color, FILLED);
		circle(image, Point2f(x5, y5), 3, color, FILLED);
		circle(image, Point2f(x6, y6), 3, color, FILLED);

		imshow("window", image);
		
		whatNow = waitKey(5000);

		if (whatNow == 27) { cap.release(); destroyAllWindows(); break; }
	}

	return(0);
}