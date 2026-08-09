import secrets
from datetime import timedelta
from django.shortcuts import render, redirect
from django.contrib import messages
from django.contrib.auth.hashers import check_password
from django.db.models import Count
from django.utils import timezone
from django.urls import reverse
from django.core.mail import send_mail
from django.conf import settings
from .models import Student, Course, Enrollment, Instructor, Score, Notification, Grading, Subject


def _with_seats(queryset):
    queryset = queryset.annotate(enrolled_count=Count('enrollment'))
    for c in queryset:
        c.seats_left = max(c.seats_limit - c.enrolled_count, 0)
    return queryset


def home(request):
    courses = _with_seats(Course.objects.select_related('instructor').all())[:3]
    for c in courses:
        c.subjects = Subject.objects.filter(coursedetail__course=c)
    return render(request, "home.html", {"courses": courses})


def about(request):
    return render(request, "about.html")


def courses(request):
    course_list = _with_seats(Course.objects.select_related('instructor').all())
    for c in course_list:
        c.subjects = Subject.objects.filter(coursedetail__course=c)
    all_subjects = Subject.objects.all()
    return render(request, "courses.html", {"courses": course_list, "all_subjects": all_subjects})


def courses_by_subject(request, subject_id):
    subject = Subject.objects.filter(pk=subject_id).first()
    if not subject:
        messages.error(request, "That subject doesn't exist.")
        return redirect('courses')

    course_list = _with_seats(
        Course.objects.select_related('instructor').filter(coursedetail__subject=subject)
    )
    for c in course_list:
        c.subjects = Subject.objects.filter(coursedetail__course=c)
    all_subjects = Subject.objects.all()

    return render(request, "courses.html", {
        "courses": course_list,
        "all_subjects": all_subjects,
        "active_subject": subject,
    })


def enrollment(request):
    student_id = request.session.get('student_id')
    if not student_id:
        messages.info(request, "Please log in to view your enrollments.")
        return redirect('login')

    student = Student.objects.filter(pk=student_id).first()
    if not student:
        request.session.flush()
        messages.error(request, "Your session expired. Please log in again.")
        return redirect('login')

    enrollments = Enrollment.objects.filter(student=student).select_related('course')
    scores = Score.objects.filter(student=student).select_related('course', 'subject').order_by('course__name', 'subject__name')

    scored_course_ids = set(scores.values_list('course_id', flat=True))

    grade_rows = []
    all_score_values = []
    for s in scores:
        all_score_values.append(s.score)
        grade_band = Grading.objects.filter(min_score__lte=s.score, max_score__gte=s.score).first()
        grade_rows.append({
            "course": s.course,
            "subject": s.subject,
            "score": s.score,
            "letter_grade": grade_band.des if grade_band else "N/A",
        })

    overall_average = round(sum(all_score_values) / len(all_score_values), 1) if all_score_values else None

    enrollment_rows = []
    pending_count = 0
    for e in enrollments:
        has_grades = e.course_id in scored_course_ids
        if not has_grades:
            pending_count += 1
        enrollment_rows.append({
            "enrollment_id": e.enrollment_id,
            "course": e.course,
            "approval_status": e.status,
            "has_grades": has_grades,
        })

    return render(request, "enrollment.html", {
        "student": student,
        "enrollment_rows": enrollment_rows,
        "grade_rows": grade_rows,
        "total_courses": enrollments.count(),
        "overall_average": overall_average,
        "pending_count": pending_count,
    })


def unenroll_course(request, enrollment_id):
    student_id = request.session.get('student_id')
    if not student_id:
        return redirect('login')

    deleted, _ = Enrollment.objects.filter(pk=enrollment_id, student_id=student_id).delete()
    if deleted:
        messages.success(request, "You've been unenrolled from that course.")
    else:
        messages.error(request, "That enrollment couldn't be found.")
    return redirect('enrollment')


def course_enrollment(request):
    student_id = request.session.get('student_id')
    if not student_id:
        messages.info(request, "Please log in to enroll in a course.")
        return redirect('login')

    student = Student.objects.filter(pk=student_id).first()
    if not student:
        request.session.flush()
        messages.error(request, "Your session expired. Please log in again.")
        return redirect('login')

    if request.method == "POST":
        course_id = request.POST.get("course_id")
        course = Course.objects.filter(pk=course_id).first()

        if not course:
            messages.error(request, "Invalid course selected.")
        else:
            already_enrolled = Enrollment.objects.filter(student=student, course=course).exists()

            if not already_enrolled:
                occupied_seats = Enrollment.objects.filter(course=course).exclude(status='Rejected').count()
                if occupied_seats >= course.seats_limit:
                    messages.error(request, f'Sorry, "{course.name}" is full. No seats left.')
                    return redirect("course_enrollment")

            enrollment_obj, created = Enrollment.objects.get_or_create(
                student=student,
                course=course,
                defaults={"fees": course.fees},
            )

            if created:
                messages.success(request, "Enrollment successful! We'll confirm your payment shortly.")
            else:
                messages.info(request, "You're already enrolled in this course.")
        return redirect("course_enrollment")

    course_list = Course.objects.all()
    return render(
        request,
        "course_enrollment.html",
        {"courses": course_list, "student": student},
    )


LOCKOUT_THRESHOLD = 5
LOCKOUT_DURATION_MINUTES = 15
RESET_TOKEN_VALID_MINUTES = 60


def student_login(request):
    if request.method == "POST":
        email = request.POST.get("email")
        password = request.POST.get("password")

        student = Student.objects.filter(email=email).first()

        if student and student.locked_until and student.locked_until > timezone.now():
            minutes_left = int((student.locked_until - timezone.now()).total_seconds() / 60) + 1
            messages.error(request, f"Too many failed attempts. Try again in {minutes_left} minute(s).")
            return render(request, "login.html")

        if student and check_password(password, student.password):
            if not student.is_verified:
                messages.error(request, "Please verify your email before logging in. Check your inbox.")
                return render(request, "login.html")

            student.failed_login_attempts = 0
            student.locked_until = None
            student.save()
            request.session["student_id"] = student.student_id
            return redirect("home")

        if student:
            student.failed_login_attempts += 1
            if student.failed_login_attempts >= LOCKOUT_THRESHOLD:
                student.locked_until = timezone.now() + timedelta(minutes=LOCKOUT_DURATION_MINUTES)
                student.failed_login_attempts = 0
                messages.error(request, f"Too many failed attempts. Your account is locked for {LOCKOUT_DURATION_MINUTES} minutes.")
            else:
                messages.error(request, "Invalid email or password.")
            student.save()
        else:
            messages.error(request, "Invalid email or password.")

    return render(request, "login.html")


def forgot_password(request):
    if request.method == "POST":
        email = request.POST.get("email")
        student = Student.objects.filter(email=email).first()

        if student:
            student.reset_token = secrets.token_urlsafe(32)
            student.reset_token_created_at = timezone.now()
            student.save()

            reset_url = request.build_absolute_uri(
                reverse('reset_password', args=[student.reset_token])
            )
            send_mail(
                subject="Reset your password - YHA Class",
                message=(
                    f"Hi {student.name},\n\n"
                    f"Click the link below to reset your password. This link expires in "
                    f"{RESET_TOKEN_VALID_MINUTES} minutes.\n\n{reset_url}\n\n"
                    f"If you didn't request this, you can ignore this email.\n\n"
                    f"- YHA Class Team"
                ),
                from_email=settings.DEFAULT_FROM_EMAIL,
                recipient_list=[student.email],
                fail_silently=True,
            )

        # Same message regardless of whether the email exists, to avoid revealing registered emails
        messages.success(request, "If that email is registered, a reset link has been sent.")
        return redirect('login')

    return render(request, "forgot_password.html")


def reset_password(request, token):
    student = Student.objects.filter(reset_token=token).first()

    if not student or not student.reset_token_created_at:
        messages.error(request, "That reset link is invalid or has already been used.")
        return redirect('login')

    if timezone.now() > student.reset_token_created_at + timedelta(minutes=RESET_TOKEN_VALID_MINUTES):
        messages.error(request, "That reset link has expired. Please request a new one.")
        return redirect('forgot_password')

    if request.method == "POST":
        new_password = request.POST.get("new_password")
        confirm_password = request.POST.get("confirm_password")

        if not new_password or new_password != confirm_password:
            messages.error(request, "Passwords do not match.")
            return render(request, "reset_password.html", {"token": token})

        student.set_password(new_password)
        student.reset_token = ""
        student.reset_token_created_at = None
        student.save()

        messages.success(request, "Your password has been reset. Please log in.")
        return redirect('login')

    return render(request, "reset_password.html", {"token": token})


def student_logout(request):
    request.session.flush()
    messages.success(request, "You have been logged out.")
    return redirect('home')


def student_context(request):
    """Context processor: makes `current_student` and notifications available in every template."""
    student = None
    unread_notifications_count = 0
    latest_notifications = []
    student_id = request.session.get('student_id')
    if student_id:
        student = Student.objects.filter(pk=student_id).first()
        if student:
            unread_notifications_count = Notification.objects.filter(student=student, is_read=False).count()
            latest_notifications = Notification.objects.filter(student=student)[:5]
    return {
        'current_student': student,
        'unread_notifications_count': unread_notifications_count,
        'latest_notifications': latest_notifications,
    }


def mark_notifications_read(request):
    student_id = request.session.get('student_id')
    if student_id:
        Notification.objects.filter(student_id=student_id, is_read=False).update(is_read=True)
    next_url = request.GET.get('next') or 'home'
    return redirect(next_url)


def clear_all_notifications(request):
    student_id = request.session.get('student_id')
    if student_id:
        Notification.objects.filter(student_id=student_id).delete()
    next_url = request.POST.get('next') or request.GET.get('next') or 'home'
    return redirect(next_url)


def clear_notification(request, notification_id):
    student_id = request.session.get('student_id')
    if student_id:
        # filtering by student_id too ensures a student can only delete their own notification
        Notification.objects.filter(pk=notification_id, student_id=student_id).delete()
    next_url = request.POST.get('next') or request.GET.get('next') or 'home'
    return redirect(next_url)


def register(request):
    if request.method == "POST":
        name = request.POST.get("name")
        full_name = request.POST.get("full_name")
        email = request.POST.get("email")
        password = request.POST.get("password")
        confirm_password = request.POST.get("confirm_password")

        if not all([name, full_name, email, password, confirm_password]):
            messages.error(request, "Please fill in all fields.")
            return render(request, "register.html")

        if password != confirm_password:
            messages.error(request, "Passwords do not match.")
            return render(request, "register.html")

        if Student.objects.filter(email=email).exists():
            messages.error(request, "An account with this email already exists.")
            return render(request, "register.html")

        student = Student(name=name, full_name=full_name, email=email)
        student.set_password(password)
        student.verification_token = secrets.token_urlsafe(32)
        student.save()

        verify_url = request.build_absolute_uri(
            reverse('verify_email', args=[student.verification_token])
        )
        send_mail(
            subject="Verify your email - YHA Class",
            message=(
                f"Hi {student.name},\n\n"
                f"Please verify your email by clicking the link below:\n{verify_url}\n\n"
                f"If you didn't create this account, you can ignore this email.\n\n"
                f"- YHA Class Team"
            ),
            from_email=settings.DEFAULT_FROM_EMAIL,
            recipient_list=[student.email],
            fail_silently=True,
        )

        messages.success(request, "Account created! Check your email for a verification link before logging in.")
        return redirect("login")

    return render(request, "register.html")


def verify_email(request, token):
    student = Student.objects.filter(verification_token=token).first()
    if student and token:
        student.is_verified = True
        student.verification_token = ""
        student.save()
        messages.success(request, "Your email has been verified. You can now log in.")
    else:
        messages.error(request, "That verification link is invalid or has already been used.")
    return redirect('login')


def edit_profile(request):
    student_id = request.session.get('student_id')
    if not student_id:
        return redirect('login')

    student = Student.objects.filter(pk=student_id).first()
    if not student:
        request.session.flush()
        return redirect('login')

    if request.method == "POST":
        name = request.POST.get("name")
        full_name = request.POST.get("full_name")
        email = request.POST.get("email")
        current_password = request.POST.get("current_password")
        new_password = request.POST.get("new_password")
        confirm_new_password = request.POST.get("confirm_new_password")
        profile_picture = request.FILES.get("profile_picture")
        next_url = request.POST.get("next") or "home"

        if Student.objects.filter(email=email).exclude(pk=student.pk).exists():
            messages.error(request, "That email is already used by another account.")
            return redirect(next_url)

        if new_password or confirm_new_password or current_password:
            if not check_password(current_password, student.password):
                messages.error(request, "Current password is incorrect. No changes were saved.")
                return redirect(next_url)
            if new_password != confirm_new_password:
                messages.error(request, "New passwords do not match. No changes were saved.")
                return redirect(next_url)
            if new_password:
                student.set_password(new_password)

        student.name = name
        student.full_name = full_name
        student.email = email
        if profile_picture:
            student.profile_picture = profile_picture
        student.save()

        messages.success(request, "Profile updated successfully.")
        return redirect(next_url)

    return redirect('home')


def admin_db(request):
    if not request.user.is_authenticated or not request.user.is_staff:
        messages.error(request, "You don't have permission to access this page.")
        return redirect('home')

    if request.method == "POST":
        action_type = request.POST.get('action_type')

        if action_type == "add_course":
            title = request.POST.get('course_title')
            period = request.POST.get('course_period')
            seats = request.POST.get('course_seats')
            fee = request.POST.get('course_fee')
            inst_id = request.POST.get('instructor_id')
            desc = request.POST.get('course_desc')

            instructor_obj = Instructor.objects.filter(pk=inst_id).first() if inst_id else None

            Course.objects.create(
                name=title,
                description=desc or "",
                period=period,
                fees=fee,
                seats_limit=seats or 40,
                instructor=instructor_obj,
            )
            messages.success(request, "Course added successfully.")
            return redirect('admin_db')

    instructors = Instructor.objects.all()
    return render(request, 'admin_db.html', {'instructors': instructors})