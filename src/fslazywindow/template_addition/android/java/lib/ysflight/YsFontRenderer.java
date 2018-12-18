package lib.ysflight;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.Log;

public class YsFontRenderer {
    public static int [] RenderFont(int fontSize,int color,String str)
    {
        // Based on https://stackoverflow.com/questions/8799290/convert-string-text-to-bitmap
        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setTextSize(fontSize);
        paint.setColor(color);
        paint.setTextAlign(Paint.Align.LEFT);
        float baseline = -paint.ascent();
        int width = (int) (paint.measureText(str)) + 1;
        int height = (int) (baseline + paint.descent()) + 1;
        Bitmap image = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(image);
        canvas.drawText(str, 0, baseline, paint);

        int pixmap[] = new int[2+width * height];
        pixmap[0]=width;
        pixmap[1]=height;
        image.getPixels(pixmap, 2, width, 0, 0, width, height);

        Log.i("FontRenderer","test");

        return pixmap;
    }
}
