from django.contrib import admin
from django.core.mail import send_mail
from django.conf import settings
from .models import (
    Student, Course, Subject, CourseDetail, Enrollment,
    Instructor, SubjectDetail, Grading, Score, Notification
)


class CourseDetailInline(admin.TabularInline):
    model = CourseDetail
    extra = 1


class CourseAdmin(admin.ModelAdmin):
    list_display = ('name', 'period', 'start_date', 'fees', 'instructor')
    inlines = [CourseDetailInline]

    def save_model(self, request, obj, form, change):
        is_new = not change
        super().save_model(request, obj, form, change)

        if is_new:
            students = Student.objects.all()
            Notification.objects.bulk_create([
                Notification(
                    student=s,
                    message=f'New class "{obj.name}" is now open for enrollment!'
                ) for s in students
            ])


class EnrollmentAdmin(admin.ModelAdmin):
    list_display = ('student', 'course', 'status', 'date')
    list_filter = ('status',)

    def save_model(self, request, obj, form, change):
        old_status = None
        if change:
            old_status = Enrollment.objects.get(pk=obj.pk).status

        super().save_model(request, obj, form, change)

        if obj.status != old_status and obj.status in ('Approved', 'Rejected'):
            if obj.status == 'Approved':
                message = f'Your enrollment in "{obj.course.name}" has been approved!'
                email_subject = "Enrollment Approved - YHA Class"
                email_body = (
                    f"Hi {obj.student.name},\n\n"
                    f'Your enrollment in "{obj.course.name}" has been approved. Welcome aboard!\n\n'
                    f"- YHA Class Team"
                )
            else:
                message = f'Your enrollment in "{obj.course.name}" was not approved.'
                email_subject = "Enrollment Update - YHA Class"
                email_body = (
                    f"Hi {obj.student.name},\n\n"
                    f'Unfortunately your enrollment in "{obj.course.name}" was not approved. '
                    f"Please contact us if you have any questions.\n\n"
                    f"- YHA Class Team"
                )

            Notification.objects.create(student=obj.student, message=message)
            send_mail(
                subject=email_subject,
                message=email_body,
                from_email=settings.DEFAULT_FROM_EMAIL,
                recipient_list=[obj.student.email],
                fail_silently=True,
            )


class ScoreAdmin(admin.ModelAdmin):
    list_display = ('student', 'course', 'subject', 'score')

    def save_model(self, request, obj, form, change):
        is_new = not change
        super().save_model(request, obj, form, change)

        if is_new:
            Notification.objects.create(
                student=obj.student,
                message=f'Your exam result for "{obj.subject.name}" in "{obj.course.name}" is now out! Score: {obj.score}/100'
            )


class SubjectAdmin(admin.ModelAdmin):
    list_display = ('name',)
    inlines = [CourseDetailInline]


admin.site.register(Student)
admin.site.register(Course, CourseAdmin)
admin.site.register(Subject, SubjectAdmin)
admin.site.register(CourseDetail)
admin.site.register(Enrollment, EnrollmentAdmin)
admin.site.register(Instructor)
admin.site.register(SubjectDetail)
admin.site.register(Grading)
admin.site.register(Score, ScoreAdmin)
admin.site.register(Notification)