from django.db import models
from django.contrib.auth.hashers import make_password


class Student(models.Model):
    student_id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=100)
    password = models.CharField(max_length=128)  # stores a hash, never plaintext
    full_name = models.CharField(max_length=100)
    email = models.EmailField(unique=True)
    profile_picture = models.ImageField(upload_to='profile_pics/', null=True, blank=True)

    is_verified = models.BooleanField(default=False)
    verification_token = models.CharField(max_length=64, blank=True, default="")

    reset_token = models.CharField(max_length=64, blank=True, default="")
    reset_token_created_at = models.DateTimeField(null=True, blank=True)

    failed_login_attempts = models.IntegerField(default=0)
    locked_until = models.DateTimeField(null=True, blank=True)

    def set_password(self, raw_password):
        self.password = make_password(raw_password)

    def __str__(self):
        return self.name


class Instructor(models.Model):
    instructor_id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=150)
    email = models.EmailField(unique=True)
    phone = models.CharField(unique=True, max_length=20)

    GENDER_CHOICES = [
        ("M", "MALE"),
        ("F", "FEMALE"),
        ("O", "OTHERS"),
    ]
    gender = models.CharField(max_length=1, choices=GENDER_CHOICES)

    def __str__(self):
        return self.name


class Course(models.Model):
    course_id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=150)
    description = models.TextField(blank=True, default="")
    period = models.CharField(max_length=50)
    fees = models.FloatField()
    start_date = models.DateField(null=True, blank=True)
    created_at = models.DateTimeField(auto_now_add=True)
    instructor = models.ForeignKey(Instructor, on_delete=models.SET_NULL, null=True, blank=True)
    seats_limit = models.IntegerField(default=40)

    def __str__(self):
        return self.name


class Subject(models.Model):
    subject_id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=100, null=False)

    def __str__(self):
        return self.name


class CourseDetail(models.Model):
    course = models.ForeignKey(Course, on_delete=models.CASCADE)
    subject = models.ForeignKey(Subject, on_delete=models.CASCADE)

    class Meta:
        unique_together = ('course', 'subject')

    def __str__(self):
        return f"{self.course.name} - {self.subject.name}"


class Enrollment(models.Model):
    STATUS_CHOICES = [
        ('Pending', 'Pending'),
        ('Approved', 'Approved'),
        ('Rejected', 'Rejected'),
    ]

    enrollment_id = models.AutoField(primary_key=True)
    student = models.ForeignKey(Student, on_delete=models.CASCADE)
    course = models.ForeignKey(Course, on_delete=models.CASCADE)
    date = models.DateTimeField(auto_now_add=True)
    fees = models.FloatField(null=False)
    status = models.CharField(max_length=10, choices=STATUS_CHOICES, default='Pending')

    class Meta:
        unique_together = ('student', 'course')

    def __str__(self):
        return f"{self.student.name} -> {self.course.name}"


class SubjectDetail(models.Model):
    instructor = models.ForeignKey(Instructor, on_delete=models.CASCADE)
    subject = models.ForeignKey(Subject, on_delete=models.CASCADE)

    class Meta:
        unique_together = ('instructor', 'subject')

    def __str__(self):
        return f"{self.instructor.name} teaches {self.subject.name}"


class Grading(models.Model):
    grading_id = models.AutoField(primary_key=True)
    des = models.CharField(max_length=50)  # e.g. "A", "Excellent"
    min_score = models.IntegerField()
    max_score = models.IntegerField()

    class Meta:
        ordering = ['-min_score']

    def __str__(self):
        return f"{self.des} ({self.min_score}-{self.max_score})"


class Score(models.Model):
    student = models.ForeignKey(Student, on_delete=models.CASCADE)
    subject = models.ForeignKey(Subject, on_delete=models.CASCADE)
    course = models.ForeignKey(Course, on_delete=models.CASCADE)
    score = models.IntegerField()

    class Meta:
        unique_together = ('student', 'subject', 'course')

    def __str__(self):
        return f"{self.student.name} - {self.subject.name}: {self.score}"


class Notification(models.Model):
    notification_id = models.AutoField(primary_key=True)
    student = models.ForeignKey(Student, on_delete=models.CASCADE)
    message = models.CharField(max_length=255)
    is_read = models.BooleanField(default=False)
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        ordering = ['-created_at']

    def __str__(self):
        return f"{self.student.name}: {self.message[:40]}"