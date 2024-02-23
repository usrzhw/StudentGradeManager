var classes;
window.onload = function () {
    fetch("/app/stu.get_student_subjects",
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
            classes = json_data;
            for (var it of classes.subjects) {
                var teachers = "";
                for (var teacher of it.teachers) {
                    teachers = teachers + teacher + " ";
                }
                var str = "" +
                    "<tr>" +
                    "<td>" + it.name + "</td>" +
                    "<td>" + it.ID + "</td>" +
                    "<td>" + it.classroom + "</td>" +
                    "<td>" + teachers + "</td>" +
                    "<td>" + it.start + "-" + it.end + "-" + it.semester + "</td>" +
                    "</tr>";
                document.getElementById("subjects_from_body").innerHTML += str;
            }
        }).catch(error => {
        });
}

function on_start_year_input() {
    var end_input = document.getElementById("end_year");
    if (document.getElementById("start_year").value != "") {
        end_input.value = Number(document.getElementById("start_year").value) + 1;
    }
    else end_input.value = "";
    search();
}
function on_end_year_intput() {
    var end_input = document.getElementById("start_year");
    if (document.getElementById("end_year").value != "") {
        end_input.value = Number(document.getElementById("end_year").value) - 1;
    }
    else end_input.value = "";
    search();
}

function search() {
    document.getElementById("subjects_from_body").innerHTML = "";
    for (var it of classes.subjects) {
        if ((it.start == Number(document.getElementById("start_year").value) &&
            it.end == Number(document.getElementById("end_year").value)) ||
            document.getElementById("end_year").value == "") {
            var teachers = "";
            for (var teacher of it.teachers) {
                teachers = teachers + teacher + " ";
            }
            var str = "" +
                "<tr>" +
                "<td>" + it.name + "</td>" +
                "<td>" + it.ID + "</td>" +
                "<td>" + it.classroom + "</td>" +
                "<td>" + teachers + "</td>" +
                "<td>" + it.start + "-" + it.end + "-" + it.semester + "</td>" +
                "</tr>";
            document.getElementById("subjects_from_body").innerHTML += str;
        }
    }
}
