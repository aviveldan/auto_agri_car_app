package com.example.kai.mulitactivityapp;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.util.Pair;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Stack;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by Kai on 01/03/2015.
 */
public class PathDrawView extends View {

    private final int DEFAULT_SCALE = this.getWidth();
    private static int LOOP_DISTANCE = 2000;
	Point[] results;
    private int VIEW_DIMENSION = 0;
    static HashSet<Pair<Point,Integer>> stopPoints = new HashSet();
    private Bitmap leftPlantImage;

    enum Commands {
        FORWARD,
        TURN_RIGHT,
        TURN_LEFT,
        STOP,
        WATER_PLANT
    }

    private static int EDIT_DISTANCE = 30;
    private static final double EXP = 3;
    private static final double ANGLE_THRESHOLD = Math.toRadians(0.5f);
    private static final int MAX_POINTS = 0;
    private static double EPSILON = 3;
    private volatile Stack<Point> points = new Stack<>();
    private Paint pathPaint,errorPaint, gridPaint, yellowPaint;
    private static final int darkGray = Color.argb(255,70,70,70);
    private static final int yellow = Color.argb(255, 255, 225, 0);
    private static final int MIN_DISTANCE = 5;
    private int obstacleIndex = -1;
    List<String> stringList;
    private SharedPreferences sharedPreferences;

    private Stack<Point> pointsValidated = new Stack<>();
    private Stack<Point> pointsRaw = new Stack<>();
    private Stack<Point> plantsPoints = new Stack<>();
    boolean validated = false;
    private static Bitmap givenBackgroundBitmap = null;
    private Button swapButton;
    Bitmap rightPlantImage = null;
    Bitmap plantImage = null;
    public static void setGivenBackgroundBitmap(Bitmap givenBackgroundBitmap) {
        PathDrawView.givenBackgroundBitmap = givenBackgroundBitmap;
    }

    // private Walker walker = new Walker(300,300,Math.PI/6,3.5,Math.PI/60);

    public void onObstacleDetected(int instructionIndex){
        obstacleIndex = (instructionIndex+1)/2+1;
    }

    public void resetObstacleDetected(){
        obstacleIndex = -1;
    }


    public PathDrawView(Context context) {
        super(context);
        init(context);
    }

    public PathDrawView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public PathDrawView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init(context);
    }

    @Override
    public void onSizeChanged(int w, int h, int prevW, int prevH){
        super.onSizeChanged(w, h, prevW, prevH);

        VIEW_DIMENSION = this.getWidth();
        System.out.println("the width is: " + VIEW_DIMENSION);
    }

    public Context myContext;

    private void init(Context context) {
        Resources res = getResources();
        givenBackgroundBitmap = BitmapFactory.decodeResource(res, R.drawable.grid2);
        rightPlantImage = BitmapFactory.decodeResource(getResources(), R.drawable.right_plant);
        leftPlantImage = BitmapFactory.decodeResource(getResources(), R.drawable.left_plant);
        plantImage = BitmapFactory.decodeResource(getResources(), R.drawable.plant);


        myContext = context;
        pathPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        pathPaint.setStyle(Paint.Style.STROKE);
        pathPaint.setStrokeWidth(6);
        pathPaint.setShadowLayer(10, 0, 0, Color.BLACK);

        gridPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        gridPaint.setStyle(Paint.Style.STROKE);
        gridPaint.setStrokeWidth(6);
        gridPaint.setColor(Color.LTGRAY);

        yellowPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        yellowPaint.setStyle(Paint.Style.STROKE);
        yellowPaint.setStrokeWidth(6);
        yellowPaint.setColor(Color.YELLOW);

        errorPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        errorPaint.setStyle(Paint.Style.STROKE);
        errorPaint.setStrokeWidth(6);
        errorPaint.setShadowLayer(10, 0, 0, Color.BLACK);
        errorPaint.setColor(Color.RED);

        this.setLayerType(View.LAYER_TYPE_SOFTWARE, null);
        Timer t;
        t = new Timer();
        t.scheduleAtFixedRate(new TimerTask() {
            @Override
            public void run() {
                if(errorPaint.getColor()==Color.RED){
                    errorPaint.setColor(darkGray);
                }else{
                    errorPaint.setColor(Color.RED);
                }
                postInvalidate();
            }
        },400,400);

        ///------------initialization point of the car-------------
        //this.addPoint(100, 100);
    }
    private void invalidateScreen(){
        this.invalidate();
    }

    protected void onDraw(Canvas canvas) {
        int height = getHeight();
        int width = getWidth();
        int rowsCount = height/100;
        int columnsCount = width/100;
        //borders
        canvas.drawLine(0, 0, width, 0, gridPaint);
        canvas.drawLine(0, 0, 0, height, gridPaint);
        //lines
        for (int i = 0; i < rowsCount; ++i) {
            canvas.drawLine(0, height / rowsCount * (i + 1), width, height / rowsCount * (i + 1), gridPaint);
        }
        for (int i = 0; i < columnsCount; ++i) {
            canvas.drawLine(width / columnsCount * (i + 1), 0, width / columnsCount * (i + 1), height, gridPaint);
        }
        super.onDraw(canvas);

        Stack<Point> points;

        points = pointsRaw;


        //draw plants
        for(Point p : plantsPoints){
            Pair<Point,Integer> checkStopRight = new Pair(p,Commands.TURN_RIGHT.ordinal());
            Pair<Point,Integer> checkStopLeft = new Pair(p,Commands.TURN_LEFT.ordinal());
            Pair<Point,Integer> checkStopStraight = new Pair(p,Commands.WATER_PLANT.ordinal());
            boolean stopLeft = stopPoints.contains(checkStopLeft);
            boolean stopRight = stopPoints.contains(checkStopRight);
            boolean stopStraight = stopPoints.contains(checkStopStraight);
            if(stopLeft) canvas.drawBitmap(leftPlantImage, p.x-70, p.y-70, null);
            if(stopRight) canvas.drawBitmap(rightPlantImage, p.x-70, p.y-70, null);
            if(stopStraight) canvas.drawBitmap(plantImage, p.x-70, p.y-70, null);

        }



        if (points.size() > 1) {
            Point prev = points.firstElement();
            int i = 0;
            for (Point p : points) {
                if(i++== obstacleIndex){
                    canvas.drawLine(p.x, p.y, prev.x, prev.y, errorPaint);
                }else{
                    canvas.drawLine(p.x, p.y, prev.x, prev.y, pathPaint);
                    canvas.drawCircle(p.x, p.y, 5, yellowPaint);
                }
                prev = p;
            }
        }
    }

    public void addPoint(float x, float y) {
        pointsRaw.add(new Point((int) x, (int) y));
        this.invalidate();
    }

    public void deletePoints() {
        pointsRaw.removeAllElements();
        plantsPoints.removeAllElements();
        this.invalidate();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();


        String mode = PreferenceManager.getDefaultSharedPreferences(myContext).getString("DrawMode", "none");
        if(mode.startsWith("PlantMode")){
            onTouchPlantMode(event);
            return true;
        }

        else if (action == MotionEvent.ACTION_DOWN) {
            this.addPoint(event.getX(), event.getY());
        }

        else if (action == MotionEvent.ACTION_UP) {
            if(pointsRaw.size() < 2) return true;
            this.validateLine();
            validated = true;
            this.pathPaint.setColor(darkGray);
        }

        return true;
    }

    public void onTouchPlantMode(MotionEvent event) {
        int action = event.getActionMasked();
        if (action != MotionEvent.ACTION_DOWN) {
            return;
        }
        Point orig = new Point((int)event.getX(), (int)event.getY());
        Point closest = getClosestVerticeOnPath(orig);
        plantsPoints.add(closest);
        String mode = PreferenceManager.getDefaultSharedPreferences(myContext).getString("DrawMode", "none");
        int stopCommand = 0;
        if(mode.equals("PlantModeRight")){
            stopCommand = Commands.TURN_RIGHT.ordinal();
        }
        if(mode.equals("PlantModeLeft")){
            stopCommand = Commands.TURN_LEFT.ordinal();
        }
        if(mode.equals("PlantModeStraight")){
            stopCommand = Commands.WATER_PLANT.ordinal();
        }
        Pair p = new Pair(closest, stopCommand);
        stopPoints.add(p);

        this.validateLine();
    }

    private Point getClosestVerticeOnPath(Point B) {
        double minDist = Double.MAX_VALUE;
        Point minP = pointsRaw.get(0);
        for(int i = 0;i<pointsRaw.size()-1;i++){
            Point A = pointsRaw.get(i);
            int d = (int)Distance(A,B);
            if(d<=minDist){
                minP = A;
                minDist = d;
            }
        }
        return minP;
    }

    public synchronized void validateLine() {
        this.validated = false;

 		String loopValue = sharedPreferences.getString("LOOP_LIST","testString2");

        if(loopValue.compareTo("testString1") == 0){
            LOOP_DISTANCE = 0;
        }
        if(loopValue.compareTo("testString2") == 0){
            LOOP_DISTANCE = 1000;
        }
        if(loopValue.compareTo("testString3") == 0){
            LOOP_DISTANCE = 2000;
        }


        int D = getDispSquared(pointsRaw);
             if (D < LOOP_DISTANCE) pointsValidated = forceLoop(pointsRaw);
        else{
            pointsValidated = getPointListClone(pointsRaw);
        }


        while(pointsValidated.size()<MAX_POINTS) {
            pointsValidated = subdivide(pointsValidated);
        }

        //<TO DO> Cast point list to array, run simplifyLines, cast back to Stack/List
        int pSize = pointsValidated.size();

        Point[] tmp = new Point[pointsValidated.size()];
        for(int i=0;i<tmp.length;i++){
            tmp[i] = pointsValidated.get(i);
        }

        //Point[] newPoints = simplifyLines(tmp);
        //List<Point> list = Arrays.asList(newPoints);
        List<Point> list = Arrays.asList(tmp);
        pointsValidated.removeAllElements();
        pointsValidated.addAll(list);

        for(int i=0;i<pointsValidated.size()-2;i++){
            if(Distance(pointsValidated.get(i), pointsValidated.get(i+1))<MIN_DISTANCE){
                pointsValidated.remove(i+1);
                i--;
            }
        }

        stringList = toStringList(pointsValidated, DEFAULT_SCALE, VIEW_DIMENSION, sharedPreferences);

        this.invalidate();
    }

    private Stack<Point> getPointListClone(Stack<Point> input) {
        Stack<Point> result = new Stack<>();
        for(Point p:input)
            result.add(new Point(p.x,p.y));
        return result;
    }


    // <TO DO> Refactor this method to take and return an array of Points.
    private synchronized Point[] simplifyLines(Point[] points) {

        /*
          If the input only consists of 2 points, return the input.
         */
        int epsilon = 3;
        String optionValue = sharedPreferences.getString("PREF_LIST", "Low");

        if(optionValue.compareTo("testValue1") == 0){
            epsilon = 0;
        }else if(optionValue.compareTo("testValue2") == 0){
            epsilon = 3;
        }else if(optionValue.compareTo("testValue3") == 0){
            epsilon = 15;
        }

        if(points.length<=2)return points;


        /*
        Check which point is the furthest from the line between the first and last element.
         */
        int dmax = 0, index = 0;
        int end = points.length-1;
        for(int i = 1;i<end;i++){
           // int d = LineToPointDistance2D(points[0],points[end],points[i]);
            int d = (int)Line2D.ptSegDist(points[0].x, points[0].y, points[end].x, points[end].y, points[i].x, points[i].y);
            if(d > dmax){
                index = i;
                dmax = d;
            }
        }


        Point[] results;

        /*
        If the max distance is larger than a set constant.
         */
        if(dmax > epsilon){

            /*
            Create new point array including the points from the first point to the point furthest
            from the line.
             */
            Point[] points2 = new Point[index+1];
            for(int i=0;i<points2.length;i++){
                points2[i] = points[i];
            }

            /*
            Recursively simplify this "first half" array.
             */
            Point[] recResult1 = simplifyLines(points2);


            /*
            Cut out the last element to account for duplication the "index" element for the merging
            of the two results.
             */
            if(recResult1.length>0)
                recResult1 = Arrays.copyOf(recResult1, recResult1.length - 1);


            /*
            Create a new point array with all the elements from the index element to the end of the
            original array.
             */
            Point[] points3 = new Point[points.length-points2.length+1];
            for(int i=0;i<points3.length;i++){
                points3[i] = points[i+index];
            }

            /*
            Recursively simplify the "second half" array.
             */
            Point[] recResult2 = simplifyLines(points3);

            /*
            Add the two arrays together...
             */
            results = new Point[recResult1.length+recResult2.length];
            for(int i=0;i<recResult1.length;i++){
                results[i] = recResult1[i];
            }
            for(int i=recResult1.length;i<results.length;i++){
                results[i] = recResult2[i-recResult1.length];
            }

        /*
        If the distance was not further than epsilon, simplify the line to a single segment from
        the first to the last point.
         */
        }else{
            results = new Point[2];
            results[0]= points[0];
            results[1]=points[points.length-1];
        }

        /*
        Return results.
         */
        return results;
    }

    private static Stack<Point> subdivide(Stack<Point> points) {
        Stack<Point> result = new Stack<>();

        //For each line segment, add point in middle, splitting line segment into two
        for(int i = 0;i<points.size()-1;i++){
            Point p1 = points.get(i);
            Point p2 = points.get(i+1);
            int x = (p1.x+p2.x)/2, y= (p1.y+p2.y)/2;
            result.add(p1);
            result.add(new Point(x,y));
        }
        if(!points.empty())
            result.add(points.lastElement());

        //For each point (Excluding final) move to the midpoint between it and the point immediately after
        for(int i = 1;i<result.size()-1;i++){
            Point p1 = result.get(i);
            Point p2 = result.get(i+1);
            int x = (p1.x+p2.x)/2, y= (p1.y+p2.y)/2;
            result.get(i).x = x;
            result.get(i).y = y;
        }
        return result;
    }

    private static Stack<Point> forceLoop(Stack<Point> input) {
        Stack<Point> points = new Stack<Point>();
        points.addAll(input);
        if(!points.empty()) {
            Point totalVector = new Point(points.peek().x - points.get(0).x, points.peek().y - points.get(0).y);
            for (int i = 0; i < points.size(); i++) {
                double ratio = Math.pow((float) (i) / (points.size() - 1), EXP);
                points.get(i).x -= totalVector.x * ratio;
                points.get(i).y -= totalVector.y * ratio;
            }
        }
        return points;
    }


    public static int getDispSquared(Stack<Point> points) {
        if(points.size()>0) {
            int dx = points.get(0).x - points.peek().x;
            int dy = points.get(0).y - points.peek().y;

            return dx * dx + dy * dy;
        }else return 0;
    }

    static List<String> toStringList(List<Point> pointList, double scale, int screenSize, SharedPreferences sharedPreferences){
        ArrayList<String> stringList = new ArrayList<String>();
        int instructionCount = (2*pointList.size())-3;
        //stringList.add(instructionCount+"!");

        String scaleValue = sharedPreferences.getString("SCALE_EDITTEXT", "120");
        try {
            scale = Double.parseDouble(scaleValue);
        }catch(Exception e){
            System.err.println(e.getMessage());
        }
        for(int i=0;i<(pointList.size()-1);i++){


            Point firstPoint = pointList.get(i);
            Point nextPoint = pointList.get(i+1);
            Point lastPoint;

            int xTwo = nextPoint.x;
            int xOne = firstPoint.x;
            int xLast;

            int yTwo = nextPoint.y;
            int yOne = firstPoint.y;
            int yLast;

            int dx1 = xTwo-xOne;
            int dy1 = yTwo-yOne;

            double vectorLengthA = Math.sqrt(Math.pow(dx1, 2) + Math.pow(dy1, 2));
            double ratio = scale/screenSize;
            vectorLengthA = ratio*vectorLengthA;

            Pair<Point,Integer> checkStopRight = new Pair(firstPoint,Commands.TURN_RIGHT.ordinal());
            Pair<Point,Integer> checkStopLeft = new Pair(firstPoint,Commands.TURN_LEFT.ordinal());
            Pair<Point,Integer> checkStopStraight = new Pair(firstPoint,Commands.WATER_PLANT.ordinal());
            boolean stopLeft = stopPoints.contains(checkStopLeft);
            boolean stopRight = stopPoints.contains(checkStopRight);
            boolean stopStraight = stopPoints.contains(checkStopStraight);
            if(stopStraight){
                stringList.add(Commands.WATER_PLANT.ordinal() + " 999 ");
            }
            if(stopRight || stopLeft){
                String firstRotate = Integer.toString(stopRight?Commands.TURN_RIGHT.ordinal():Commands.TURN_LEFT.ordinal());
                String secondRotate = Integer.toString(stopRight?Commands.TURN_LEFT.ordinal():Commands.TURN_RIGHT.ordinal());
                double rotation = 90;
                String tmpStr = firstRotate+" "+rotation+" ";
                tmpStr+=" "+ Commands.WATER_PLANT.ordinal() + " 999 ";
                tmpStr += secondRotate+" "+rotation+" ";
                stringList.add(tmpStr);
            }

            String command = Integer.toString(Commands.FORWARD.ordinal());
            String tmpStr = command+" "+(int)vectorLengthA+" "; // goForward

            stringList.add(tmpStr);

            if(i<pointList.size()-2){
                firstPoint = pointList.get(i+1);
                nextPoint = pointList.get(i+2);
                lastPoint = pointList.get(i);


                xTwo = nextPoint.x;
                xOne = firstPoint.x;
                xLast = lastPoint.x;

                yTwo = nextPoint.y;
                yOne = firstPoint.y;
                yLast = lastPoint.y;

                double rotation;

                double angleOne =(Math.atan2((yOne-yLast), (xOne-xLast)));
                double angleTwo =(Math.atan2((yTwo-yOne), (xTwo-xOne)));


                rotation = (angleTwo-angleOne);
                if(rotation>(Math.PI)) rotation = rotation-(2*(Math.PI));


                if(rotation>0){
                    String rotateCommand = Integer.toString(Commands.TURN_RIGHT.ordinal());
                    rotation = (int)Math.toDegrees(rotation);

                    tmpStr = rotateCommand+" "+rotation+" ";
                }else {
                    String rotateCommand = Integer.toString(Commands.TURN_LEFT.ordinal());
                    rotation = -(int)Math.toDegrees(rotation);
                    tmpStr =   rotateCommand+" "+rotation+" ";
                }
                stringList.add(tmpStr);
            }
        }

        return stringList;
    }


    //Compute the dot product AB . AC
    static private double DotProduct(Point A,  Point B, Point C)
    {
        double[] AB = new double[2];
        double[] BC = new double[2];
        AB[0] = B.x - A.x;
        AB[1] = B.y - A.y;
        BC[0] = C.x - B.x;
        BC[1] = C.y - B.y;
        double dot = AB[0] * BC[0] + AB[1] * BC[1];

        return dot;
    }

    //Compute the cross product AB x AC
    static  private double CrossProduct(Point A,  Point B, Point C)
    {
        double[] AB = new double[2];
        double[] AC = new double[2];
        AB[0] = B.x - A.x;
        AB[1] = B.y - A.y;
        AC[0] = C.x - A.x;
        AC[1] = C.y - A.y;
        double cross = AB[0] * AC[1] - AB[1] * AC[0];

        return cross;
    }

    //Compute the distance from A to B
    static double Distance(Point A, Point B)
    {
        double d1 = A.x - B.x;
        double d2 = A.y - B.y;

        return Math.sqrt(d1 * d1 + d2 * d2);
    }
    //Compute the distance from AB to C

    public void addSharedPreferences(SharedPreferences shPref) {
        this.sharedPreferences = shPref;
    }

    public void setswapButton(Button swapButton) {
        this.swapButton = swapButton;
    }

}





































