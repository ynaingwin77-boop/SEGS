from django.contrib import admin
from django.urls import path
from django.conf import settings
from django.conf.urls.static import static
from SEGS import views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('', views.home, name="home"),
    path('about/', views.about, name="about"),
    path('courses/', views.courses, name="courses"),
    path('courses/subject/<int:subject_id>/', views.courses_by_subject, name="courses_by_subject"),
    path('enrollment/', views.enrollment, name="enrollment"),
    path('register/', views.register, name="register"),
    path('verify-email/<str:token>/', views.verify_email, name="verify_email"),
    path('student_login/', views.student_login, name="login"),
    path('forgot-password/', views.forgot_password, name="forgot_password"),
    path('reset-password/<str:token>/', views.reset_password, name="reset_password"),
    path('logout/', views.student_logout, name="logout"),
    path('profile/edit/', views.edit_profile, name="edit_profile"),
    path('enrollment/unenroll/<int:enrollment_id>/', views.unenroll_course, name="unenroll_course"),
    path('notifications/mark-read/', views.mark_notifications_read, name="mark_notifications_read"),
    path('notifications/clear-all/', views.clear_all_notifications, name="clear_all_notifications"),
    path('notifications/clear/<int:notification_id>/', views.clear_notification, name="clear_notification"),
    path('course_enrollment/', views.course_enrollment, name="course_enrollment"),
    path('admin_db/', views.admin_db, name="admin_db"),
]

if settings.DEBUG:
    urlpatterns += static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)