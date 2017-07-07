package org.github.nsetzer.example;

import android.content.res.Resources;
import android.content.Context;
import android.app.Notification;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import org.github.nsetzer.example.R;
import android.app.Service;
import android.os.IBinder;
import android.content.Intent;
import android.os.Bundle;
import org.qtproject.qt5.android.bindings.QtService;
import android.util.Log;

public class MyCustomAppService extends QtService {

   /** Called when the service is being created. */
   @Override
   public void onCreate() {
      super.onCreate();
      Log.i("Service", "Service created!");
   }

   /** The service is starting, due to a call to startService() */
   @Override
   public int onStartCommand(Intent intent, int flags, int startId) {

      if (intent == null) {
         int ret = super.onStartCommand(intent, flags, startId);
         Log.i("Service", "Service started!");
         //showNotification();
         return ret|START_STICKY;
      }


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
      super.onDestroy();
   }

   public void showNotification() {
      Context ctx = this;
      try {

         Bitmap cover;
         Notification.Builder builder;

         Resources res = ctx.getResources();

         cover = BitmapFactory.decodeResource(res, R.drawable.ic_notif_audio);

         builder = new Notification.Builder(ctx);
         builder.setContentTitle("title")
                .setContentText("text")
                .setTicker("ticker")
                .setVisibility(Notification.VISIBILITY_PUBLIC)
                .setSmallIcon(R.drawable.ic_notif_audio);
                //.setLargeIcon(R.drawable.ic_notif_audio);

         startService(new Intent(ctx, MyCustomAppService.class));
         startForeground(3, builder.build());
      } catch (IllegalArgumentException e) {
         Log.e("Service", "Failed to display notification", e);
      }
   }

}
