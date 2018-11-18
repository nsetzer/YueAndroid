package org.github.nsetzer.yueapp2;

import android.content.res.Resources;
import android.content.Context;
import android.app.Notification;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import org.github.nsetzer.yueapp2.R;
import android.app.Service;
import android.os.IBinder;
import android.content.Intent;
import android.os.Bundle;
import org.qtproject.qt5.android.bindings.QtService;
import android.util.Log;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.app.PendingIntent;
//import android.support.v4.media.session.MediaButtonReceiver;
//import android.support.v4.media.session.MediaSessionCompat;
//import android.support.v4.media.session.PlaybackStateCompat;
//import android.support.v7.app.NotificationCompat;
import android.media.session.MediaSession;

//import android.support.annotation.MainThread;

public class YueAppService extends QtService {

   private static YueAppService m_instance;

   public static final String ACTION_PLAYBACK_NEXT = "org.github.nsetzer.yueapp2.NEXT";
   public static final String ACTION_PLAYBACK_PREV = "org.github.nsetzer.yueapp2.PREV";
   public static final String ACTION_PLAYBACK_PLAYPAUSE = "org.github.nsetzer.yueapp2.PLAYPAUSE";

   private final BroadcastReceiver mReceiver = new BroadcastReceiver() {

      @Override
      public void onReceive(Context context, Intent intent) {
         String action = intent.getAction();

         if (ACTION_PLAYBACK_PLAYPAUSE.equalsIgnoreCase(action)) {
            playpause();
         } else if (ACTION_PLAYBACK_PREV.equalsIgnoreCase(action)) {
            prev();
         } else if (ACTION_PLAYBACK_NEXT.equalsIgnoreCase(action)) {
            next();
         }

      }
   };

   public YueAppService()
   {
       m_instance = this;

   }

   /** Called when the service is being created. */
   @Override
   public void onCreate() {
      super.onCreate();
      Log.i("YueService", "Service created!");

      initMediaSession();

      IntentFilter filter = new IntentFilter();
      filter.addAction(ACTION_PLAYBACK_NEXT);
      filter.addAction(ACTION_PLAYBACK_PREV);
      filter.addAction(ACTION_PLAYBACK_PLAYPAUSE);
      registerReceiver(mReceiver, filter);

   }

   /** The service is starting, due to a call to startService() */
   @Override
   public int onStartCommand(Intent intent, int flags, int startId) {

      if (intent == null) {
         int ret = super.onStartCommand(intent, flags, startId);
         Log.i("YueService", "Service started!");
         return ret;//|START_STICKY;
      }

      String action = intent.getAction();
      if (ACTION_PLAYBACK_PLAYPAUSE.equals(action)) {
         Log.i("YueService", "new intent playpause pressed");
      } else if (ACTION_PLAYBACK_PREV.equals(action)) {
         Log.i("YueService", "new intent prev pressed");
      } else if (ACTION_PLAYBACK_NEXT.equals(action)) {
         Log.i("YueService", "new intent next pressed");
      }

      return super.onStartCommand(intent, flags, startId);
   }

   /** A client is binding to the service with bindService() */
   @Override
   public IBinder onBind(Intent intent) {
      return super.onBind(intent);
      //return mBinder;
   }

   /** Called when all clients have unbound with unbindService() */
   @Override
   public boolean onUnbind(Intent intent) {
      return super.onUnbind(intent);
   }

   /** Called when a client is binding to the service with bindService()*/
   @Override
   public void onRebind(Intent intent) {
         super.onRebind(intent);
   }

   /** Called when The service is no longer used and is being destroyed */


   @Override
   public void onDestroy() {

      stopForeground(true);
      Log.i("YueService", "stop foreground");

      unregisterReceiver(mReceiver);

      JNIServiceExtAppCallback();

      super.onDestroy();
   }

   /*
   Bitmap bitmap = BitmapFactory.decodeFile("/path/images/image.jpg");
   ByteArrayOutputStream blob = new ByteArrayOutputStream();
   bitmap.compress(CompressFormat.PNG, 0 , blob);
   byte[] bitmapdata = blob.toByteArray();
   Bitmap bitmap = BitmapFactory.decodeByteArray(bitmapdata, 0, bitmapdata.length);
   */

   // byte[] coverart, long length
   public static void showNotification( byte[] coverart, boolean playing, String title, String message) {
      Log.i("YueService", "a4 display notification");
      m_instance.showNotificationImpl(coverart, playing, title, message);
   }


   //https://developer.android.com/guide/topics/ui/notifiers/notifications.html#controllingMedia

   // String title, boolean playing, byte[] coverart, long length
   public void showNotificationImpl(byte[] coverart, boolean playing, String title, String message) {
      Context ctx = this;
      try {

         Log.i("YueService", "cover art length: " + Integer.toString(coverart.length) );

         Bitmap cover;
         Notification.Builder builder;

         Resources res = ctx.getResources();

         //cover = BitmapFactory.decodeResource(res, R.drawable.icon);
         cover = BitmapFactory.decodeByteArray (coverart,0,coverart.length);

         builder = new Notification.Builder(ctx);
         builder.setContentTitle(title)
                .setContentText(message)
                .setVisibility(Notification.VISIBILITY_PUBLIC)
                .setSmallIcon(R.drawable.icon)
                .setLargeIcon(cover);

         // open the application when the notification is clicked
         Intent appIntent = new Intent(ctx, YueAppActivity.class);
         PendingIntent appPendingIntent = PendingIntent.getActivity(
                                          ctx, 0, appIntent,
                                          Intent.FLAG_ACTIVITY_NEW_TASK);
         builder.setContentIntent(appPendingIntent);


         PendingIntent piPrev = PendingIntent.getBroadcast(ctx, 0, new Intent(ACTION_PLAYBACK_PREV), PendingIntent.FLAG_UPDATE_CURRENT);
         PendingIntent piPlayPause = PendingIntent.getBroadcast(ctx, 0, new Intent(ACTION_PLAYBACK_PLAYPAUSE), PendingIntent.FLAG_UPDATE_CURRENT);
         PendingIntent piNext = PendingIntent.getBroadcast(ctx, 0, new Intent(ACTION_PLAYBACK_NEXT), PendingIntent.FLAG_UPDATE_CURRENT);

         builder.addAction(R.drawable.ic_notif_prev, "previous", piPrev);
         if (playing) {
            builder.addAction(R.drawable.ic_notif_pause, "pause", piPlayPause);
         } else {
            builder.addAction(R.drawable.ic_notif_play, "play", piPlayPause);
         }
         builder.addAction(R.drawable.ic_notif_next, "next", piNext);

         //MediaSessionCompat.Token sessionToken = mMediaSession.getSessionToken();

         builder.setStyle(new Notification.MediaStyle()
                .setShowActionsInCompactView(0,1,2));
                //.setMediaSession(mMediaSession.getSessionToken()));

         Log.i("YueService", "display notification");
         //startService(new Intent(ctx, YueAppService.class));
         //if (playing) {
         startForeground(3, builder.build());
         //} else {
         //   stopForeground(false);
         //}
      } catch (IllegalArgumentException e) {
         Log.e("YueService", "Failed to display notification", e);
      }
   }

   //@MainThread
   public void playpause() {
      Log.i("YueService", "onbtn playpause pressed");
      JNIServicePlayPauseCallback();
   }

   //@MainThread
   public void prev() {
      Log.i("YueService", "onbtn prev pressed");
      JNIServicePrevCallback();
   }

   //@MainThread
   public void next() {
      Log.i("YueService", "onbtn next pressed");
      JNIServiceNextCallback();
   }

   private MediaSession mMediaSession;
   //protected MediaSessionCallback mSessionCallback;

   private void initMediaSession() {

        /*
        Intent mediaButtonIntent = new Intent(Intent.ACTION_MEDIA_BUTTON);

        mediaButtonIntent.setClass(this, RemoteControlClientReceiver.class);
        PendingIntent mbrIntent = PendingIntent.getBroadcast(this, 0, mediaButtonIntent, 0);
        ComponentName mbrName = new ComponentName(this, RemoteControlClientReceiver.class);

        mSessionCallback = new MediaSessionCallback();
        mMediaSession = new MediaSessionCompat(this, "VLC", mbrName, mbrIntent);
        mMediaSession.setFlags(MediaSessionCompat.FLAG_HANDLES_MEDIA_BUTTONS
                | MediaSessionCompat.FLAG_HANDLES_TRANSPORT_CONTROLS);
        mMediaSession.setCallback(mSessionCallback);
        try {
            mMediaSession.setActive(true);
        } catch (NullPointerException e) {
            // Some versions of KitKat do not support AudioManager.registerMediaButtonIntent
            // with a PendingIntent. They will throw a NullPointerException, in which case
            // they should be able to activate a MediaSessionCompat with only transport
            // controls.
            mMediaSession.setActive(false);
            mMediaSession.setFlags(MediaSessionCompat.FLAG_HANDLES_TRANSPORT_CONTROLS);
            mMediaSession.setActive(true);
        }
        setSessionToken(mMediaSession.getSessionToken());
        */
    }

    public static native void JNIServicePrevCallback();
    public static native void JNIServiceNextCallback();
    public static native void JNIServicePlayPauseCallback();
    public static native void JNIServiceExtAppCallback();


}
