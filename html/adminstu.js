
var opened_student_info;
var opened_student_id;
window.onload = ShowStudentList(); 

function up(x, y) {
    return Number(x.id) - Number(y.id);
}
async function RequestJson(url, body) {
    return fetch(url,
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: body,
            mode: 'no-cors'
        }).then(async Response => {
            if (Response.ok == false) {
                throw new Error((await Response.json()).message);
            }
            else {
                return Response.json();
            }
        });
}
async function ShowStudentList() {
    var students_info = await RequestJson("/app/stu.get_all_students_info", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
    }));
    var form = document.getElementById("FormBody");
    form.innerHTML = "";
    var str = "";
    students_info.students.sort(up);
    for (var it of students_info.students) {
        str += "<tr onclick=\"ItemClicked(this)\">" +
            "<td>" + it.id + "</td>" +
            "<td>" + it.name + "</td>" +
            "<td>" + it.sex + "</td>" +
            "<td>" + it.class + "</td>" +
            "<td>" + it.notes + "</td > " +
            "</tr>";
    }
    form.innerHTML = str;
}

function ItemClicked(e) {
    //获取被点击的元素对应的学号
    var StudentID = e.firstElementChild.textContent;
    OpenStudentInfoDialog(StudentID);
}

async function OpenStudentInfoDialog(student_id) {
    var student_info = await RequestJson("/app/stu.get_user_info", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
        target_id: student_id
    }));
    //设置dialog中各个属性的值
    document.getElementById("NameBox").value = student_info.name;
    document.getElementById("StudentIDBox").value = student_id;
    document.getElementById("SexBox").value = student_info.sex;
    document.getElementById("PhoneNumberBox").value = student_info.phone_number;
    document.getElementById("EMailBox").value = student_info.email;
    document.getElementById("CollegeBox").value = student_info.college;
    document.getElementById("ClassBox").value = student_info.class_name;
    document.getElementById("EnrollmentDateBox").value = student_info.enrollment_date;
    document.getElementById("NotesBox").value = student_info.notes;
    document.getElementById("PermissionLevelBox").value = student_info.permission_level;
    document.getElementById("PasswordBox").value = "";
    opened_student_info = student_info;
    opened_student_id = student_id;
    document.getElementById("StudentInfo").showModal();
}

function Close() {
    document.getElementById("StudentInfo").close();
    document.getElementById("AddStudentDialog").close();
}

async function Makesure() {
    //点击确定按钮后的操作
    //依次检查哪些属性被修改了
    var student_id = opened_student_id;
    if (document.getElementById("NameBox").value != opened_student_info.name) {
        //名称被修改
        await RequestJson("/app/stu.change_name", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            name: document.getElementById("NameBox").value
        })).catch(error => {
            alert("修改名称失败:" + error.toString());
        });
    }
    if (document.getElementById("SexBox").value != opened_student_info.sex) {
        await RequestJson("/app/stu.change_sex", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            sex: document.getElementById("SexBox").value
        })).catch(error => {
            alert("修改性别失败:" + error.toString());
        });
    }
    if (document.getElementById("PhoneNumberBox").value != opened_student_info.phone_number) {
        await RequestJson("/app/stu.change_phone_number", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            phone_number: document.getElementById("PhoneNumberBox").value
        })).catch(error => {
            alert("修改手机号码失败:" + error.toString());
        });
    }
    if (document.getElementById("EMailBox").value != opened_student_info.email) {
        alert("邮箱暂不支持修改");
    }
    if (document.getElementById("CollegeBox").value != opened_student_info.college) {
        await RequestJson("/app/stu.change_college", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            college: document.getElementById("CollegeBox").value
        })).catch(error => {
            alert("修改学院失败:" + error.toString());
        });
    }
    if (document.getElementById("EnrollmentDateBox").value != opened_student_info.enrollment_date) {
        await RequestJson("/app/stu.change_enrollment_date", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            enrollment_date: document.getElementById("EnrollmentDateBox").value
        })).catch(error => {
            alert("修改入学日期失败:" + error.toString());
        });
    }
    if (document.getElementById("NotesBox").value != opened_student_info.notes) {
        await RequestJson("/app/stu.change_notes", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            notes: document.getElementById("NotesBox").value
        })).catch(error => {
            alert("修改用户备注失败:" + error.toString());
        });
    }
    if (document.getElementById("PermissionLevelBox").value != opened_student_info.permission_level) {
        await RequestJson("/app/stu.change_permission_level", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            permission_level: document.getElementById("PermissionLevelBox").value
        })).catch(error => {
            alert("修改权限等级失败:" + error.toString());
        });
    }
    if (document.getElementById("PasswordBox").value != "") {
        await RequestJson("/app/stu.change_password", JSON.stringify({
            ID: localStorage.getItem("id"),
            token: localStorage.getItem("token"),
            target_id: student_id,
            password: document.getElementById("PasswordBox").value
        })).catch(error => {
            alert("修改权限等级失败:" + error.toString());
        });
    }
    Close();
    ShowStudentList();
}

function OpenAddStudentDialog() {
    document.getElementById("ANameBox").value = "";
    document.getElementById("ASexBox").value = "";
    document.getElementById("APhoneNumberBox").value = "";
    document.getElementById("AEMailBox").value = "";
    document.getElementById("ACollegeBox").value = "";
    document.getElementById("AClassBox").value = "";
    document.getElementById("AEnrollmentDateBox").value = "";
    document.getElementById("ANotesBox").value = "";
    document.getElementById("APermissionLevelBox").value = "";
    document.getElementById("APasswordBox").value = "";
    document.getElementById("AddStudentDialog").showModal();
}

async function CreateStudent() {
    await RequestJson("/app/stu.create_student", JSON.stringify({
        ID: localStorage.getItem("id"),
        token: localStorage.getItem("token"),
        name: document.getElementById("ANameBox").value,
        sex: document.getElementById("ASexBox").value,
        phone_number: document.getElementById("APhoneNumberBox").value,
        email: document.getElementById("AEMailBox").value,
        college: document.getElementById("ACollegeBox").value,
        class: document.getElementById("AClassBox").value,
        enrollment_date: document.getElementById("AEnrollmentDateBox").value,
        notes: document.getElementById("ANotesBox").value,
        permission_level: document.getElementById("APermissionLevelBox").value,
        password: document.getElementById("APasswordBox").value,
    })).catch(error => {
        alert("创建学生失败:" + error.toString());
    });
    Close();
    ShowStudentList();
}
