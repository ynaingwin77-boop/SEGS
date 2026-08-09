# YHA CLASS - Student Enrollment & Course Management System

YHA CLASS သည် Django Web Framework ကို အသုံးပြု၍ ရေးသားထားသော သင်တန်းနှင့် ကျောင်းသားမှတ်ပုံတင်ခြင်း စနစ် (Course Enrollment & Student Management System) ဖြစ်ပါသည်။

---

## 🌟 Key Features (စနစ်တွင် ပါဝင်သော လုပ်ဆောင်ချက်များ)

### 1. Student / Public Portal (ကျောင်းသားများအတွက်)
* **Home Page:** သင်တန်းကျောင်း၏ အချက်အလက်များနှင့် သင်တန်းများကို ကြည့်ရှုနိုင်ခြင်း။
* **Courses Page:** ဖွင့်လှစ်ထားသော သင်တန်းများ၊ သင်တန်းကြေး၊ သင်တန်းချိန်၊ တက်ရောက်နိုင်သော အကန့်အသတ် (Seats Left) နှင့် နည်းပြဆရာ (Instructor) အချက်အလက်များကို ကြည့်ရှုနိုင်ခြင်း။
* **Enrollment:** ကျောင်းသားများ သင်တန်းတက်ရောက်ရန် အွန်လိုင်းမှ လျှောက်ထားနိုင်ခြင်း။
* **Student Profile:** ကျောင်းသားမှ မိမိ Profile Picture၊ နာမည်၊ Password များကို ပြင်ဆင်နိုင်ခြင်း။
* **Automated Email Notifications:** Enrollment အဆင်ပြေကြောင်း အတည်ပြုစာအား SMTP မှတစ်ဆင့် ကျောင်းသား၏ Gmail သို့ အလိုအလျောက် ပေးပို့ခြင်း။

### 2. Admin Panel (စနစ်ထိန်းချုပ်သူအတွက်)
* **Course Management:** သင်တန်းအသစ်များ ထည့်သွင်းခြင်း၊ ပြင်ဆင်ခြင်း၊ ဖျက်ခြင်း။
* **Instructor Management:** သင်တန်းနည်းပြများ၏ အချက်အလက်များကို စီမံခန့်ခွဲခြင်း။
* **Enrollment Approval:** ကျောင်းသားများ၏ လျှောက်ထားလွှာများကို စစ်ဆေးအတည်ပြုခြင်း။

---

## 🛠️ Tech Stack (အသုံးပြုထားသော နည်းပညာများ)

* **Backend:** Python 3.x, Django Framework
* **Frontend:** HTML5, CSS3, JavaScript, Bootstrap 5
* **Database:** SQLite3 (Default Django Database)
* **Email Service:** SMTP (Gmail API Integration)

---

## 🚀 How to Run the Project (စက်ပေါ်တွင် စမ်းသပ် Run နည်း)

### Step 1: Repository အား Clone/Download ပြုလုပ်ပါ
```bash
git clone <your-repository-url>
cd yha_class