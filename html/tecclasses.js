window.onload = function () {
    switch_to_classes();
}

function item_click(e) {
    switch_to_students(e.textContent);
}
function switch_to_classes() {
    document.getElementById("chose_bar").innerHTML = "";
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML = "<tr><th>班级列表</th></tr>"
    fetch("http://127.0.0.1:8081/app/stu.get_teacher_classes",
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
            for (var it of json_data.classes) {
                var str = "" +
                    "<tr>" +
                    "<td onclick=\"item_click(this)\">" + it + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
            }
        }).catch(error => {
        });
}

function switch_to_students(class_name) {
    document.getElementById("FormBody").innerHTML = "";
    document.getElementById("FormHeader").innerHTML = "<tr><th>学号</th><th>姓名</th></tr>"
    document.getElementById("chose_bar").innerHTML = "<button onclick=\"switch_to_classes()\" class=\"back_button\">返回</button>"
    fetch("http://127.0.0.1:8081/app/stu.get_class_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                class_name: class_name
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            for (var it of json_data.students) {
                var str = "" +
                    "<tr onclick=\"ShowStudentInfo(this)\">" +
                    "<td>" + it.id + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "</tr>";
                document.getElementById("FormBody").innerHTML += str;
            }
        }).catch(error => {
        });
}

async function ShowStudentInfo(e) {
    document.getElementById("GradeFormBody").innerHTML = "";
    var stu_id = e.firstElementChild.textContent;
    document.getElementById("StudentInfo").innerHTML = "";
    document.getElementById("StudentGrades").showModal();
    fetch("/app/stu.get_student_subjects",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                target_id: stu_id
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
                //document.getElementById("StudentNameDiv").textContent;
                if (it.grade == -1) continue;
                var str = "" +
                    "<tr>" +
                    "<td>" + it.ID + "</td>" +
                    "<td>" + it.name + "</td>" +
                    "<td>" + it.grade + "</td>" +
                    "<td>" + it.student_note + "</td>" +
                    "</tr>";
                document.getElementById("GradeFormBody").innerHTML += str;
            }
        }).catch(error => {
        });
    fetch("/app/stu.get_user_info",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token"),
                target_id: stu_id

            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {

            var str = "" +
                "<div class=\"item\">" + "姓名: " + json_data.name + "</div>" +
                "<div class=\"item\">" + "学号: " + stu_id + "</div>" +
                "<div class=\"item\">" + "性别: " + json_data.sex + "</div>" +
                "<div class=\"item\">" + "学院: " + json_data.college + "</div>";
            document.getElementById("StudentInfo").innerHTML = str + document.getElementById("StudentInfo").innerHTML;
        }).catch(error => {
        });
}

function Close() {
    document.getElementById("StudentGrades").close();
}

