
window.onload = function () {
    switch_to_subjects();
}

function item_click(e) {
    switch_to_students(e.firstElementChild.textContent);
}


function down(x, y) {
    return y.grade - x.grade;
}
var last_open_student = 0;
var last_open_subject = 0;
function switch_to_subjects() {
    document.getElementById("ButtomDiv").textContent = "";
    document.getElementById("chose_bar").innerHTML = "";
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML ="<tr><th>课程编号</th><tr><th>开课时间</th><th>教室</th><th>课程名称</th></tr>"
    fetch("/app/stu.get_teacher_subjects",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                target_id: localStorage.getItem("id")
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            for (var it of json_data.subjects) {
                var str = "" +
                    "<tr onclick=\"item_click(this)\">" +
                    "<td>" + it.id + "</td>" +
                    "<td>" + it.semester + "</td>" +
                    "<td>"+ it.classroom + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
            }
        }).catch(error => {
        });
}

function switch_to_students(subject_id) {
    document.getElementById("ButtomDiv").textContent = "";
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML = "<tr><th>学号</th><th>姓名</th><th>成绩</th><th>备注</th></tr>"
    document.getElementById("chose_bar").innerHTML = "<div style=\"margin-left: 5vw;\">成绩-1表示未登记成绩</div><button onclick=\"switch_to_subjects()\" class=\"back_button\">返回</button>"
    fetch("/app/stu.get_subject_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                subject_id: subject_id
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            var sum = 0;
            var count = 0;
            json_data.students.sort(down);
            for (var it of json_data.students) {
                var str = "" +
                    "<tr onclick=\"SetStudentGrade(this)\">" +
                    "<td>" + it.id + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "<td>" + it.grade + "</td>" +
                    "<td>" + it.notes + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
                if (it.grade >= 0)
                    sum += it.grade, ++count;
            }
            document.getElementById("ButtomDiv").textContent = "平均成绩: " + (sum / count);
            last_open_subject = subject_id;
        }).catch(error => {
        });
}
async function SetStudentGrade(e) {
    document.getElementById("grade_box").value = e.children[2].textContent;
    var stu_id = e.firstElementChild.textContent;
    document.getElementById("note_box").value = e.children[3].textContent;
    last_open_student = e.children[0].textContent;
    document.getElementById("SetStudentInfo").showModal();
}

async function make_sure_change() {
    await fetch("/app/stu.set_student_subject_grade",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                subject_id: last_open_subject,
                student_id: last_open_student,
                grade: document.getElementById("grade_box").value
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            if (json_data.message != "ok") throw Error(json_data.message);
        }).catch(error => {
            alert("修改失败: " + error);
        });
    document.getElementById("SetStudentInfo").close();
    switch_to_students(last_open_subject);
}
function cancel_change() {
    document.getElementById("SetStudentInfo").close();
}

