package org.flaming0.df3d;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Debug;
import android.support.annotation.Keep;
import android.util.Log;
import java.util.Calendar;
import java.util.Locale;

@Keep
public class Df3dAndroidServices {
    private static String TAG = "Df3dAndroidServices";

    private Df3dLocalStorage m_storage = null;
    private Activity m_activity = null;

    private long stringToInt(String s)
    {
        if (s != null)
            return Long.valueOf(s);
        return 0;
    }

    public Df3dAndroidServices(Activity activity) {
        m_storage = new Df3dLocalStorage(activity);
        m_activity = activity;
    }

    public static void quitApp() {
        Df3dActivity.getSharedActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Df3dActivity.getSharedActivity().finish();
                System.exit(0);
            }
        });
    }

    public String getSystemLanguage() {
        return Locale.getDefault().getLanguage().toLowerCase();
    }

    public void openURL(String url) {
        MyLog.i(TAG, "openURL()");

        try {
            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
            m_activity.startActivity(intent);
        }
        catch (Exception e) {
            MyLog.e(TAG, "Failed to open URL!");
            MyLog.e(TAG, e.getMessage());
        }
    }

    public Object getLocalStorage() { return m_storage; }

    public void exitApp(/*int exitCode*/) {
        System.exit(0);
    }

    public long getProcessMemUsage() {
        if (Build.VERSION.SDK_INT >= 23) {
            Debug.MemoryInfo memInfo = new Debug.MemoryInfo();
            Debug.getMemoryInfo(memInfo);

            String javaHeap = memInfo.getMemoryStat("summary.java-heap");
            String nativeHeap = memInfo.getMemoryStat("summary.native-heap");
            String code = memInfo.getMemoryStat("summary.code");
            String stack = memInfo.getMemoryStat("summary.stack");
            String privateOther = memInfo.getMemoryStat("summary.private-other");
            String system = memInfo.getMemoryStat("summary.system");

            long resultKb = stringToInt(javaHeap) + stringToInt(nativeHeap) +
                    stringToInt(code) + stringToInt(stack) +
                    stringToInt(privateOther) + stringToInt(system);

            return resultKb << 10;
        }

        return 0;
    }

    public long getGraphicsMemUsage() {
        if (Build.VERSION.SDK_INT >= 23) {
            Debug.MemoryInfo memInfo = new Debug.MemoryInfo();
            Debug.getMemoryInfo(memInfo);

            String graphics = memInfo.getMemoryStat("summary.graphics");

            long resultKb = stringToInt(graphics);

            return resultKb << 10;
        }

        return 0;
    }

    private PendingIntent makePendingIntent(int id, String message) {
        Intent intent = new Intent(m_activity.getApplicationContext(), Df3dLocalNotificationReceiver.class);

        if (message != null)
            intent.putExtra("message", message);
        intent.putExtra("id", id);

        return PendingIntent.getBroadcast(m_activity, id, intent, PendingIntent.FLAG_UPDATE_CURRENT);
    }

    public void scheduleLocalNotification(String message, int secondsFromNow, int id) {
        PendingIntent intent = makePendingIntent(id, message);

        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(System.currentTimeMillis());
        calendar.add(Calendar.SECOND, secondsFromNow);

        AlarmManager alarmManager = (AlarmManager)m_activity.getSystemService(Context.ALARM_SERVICE);
        alarmManager.set(AlarmManager.RTC_WAKEUP, calendar.getTimeInMillis(), intent);

        MyLog.i(TAG, "Local notification scheduled");
    }

    public void cancelLocalNotification(int id) {
        MyLog.i(TAG, "cancelLocalNotification()");

        PendingIntent pendingIntent = makePendingIntent(id, null);

        AlarmManager am = (AlarmManager) m_activity.getSystemService(Context.ALARM_SERVICE);
        am.cancel(pendingIntent);

        NotificationManager manager = (NotificationManager)m_activity.getSystemService(Context.NOTIFICATION_SERVICE);
        manager.cancel(id);
    }
}
