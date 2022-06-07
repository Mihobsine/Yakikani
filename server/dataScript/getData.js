import fetch, { Headers, Request } from "node-fetch"
import fs from "fs"
import { updateEnv } from "./updateEnv.js"

const apiToken = process.env.API_TOKEN;
const lastUpdate = process.env.LAST_UPDATE;
const requestHeaders =
    new Headers({
        'Wanikani-Revision': '20170710',
        Authorization: 'Bearer ' + apiToken,
    });
const assignmentApiEndpoint =
    new Request('https://api.wanikani.com/v2/assignments' +
        '?burned=true&subject_types=kanji,vocabulary' +
        (lastUpdate === "null" ? "" : ("&updated_after=" + lastUpdate)), {
        method: 'GET',
        headers: requestHeaders
    });

var assignmentResponse = await fetch(assignmentApiEndpoint)
    .then(response => response.json())
    .then(responseBody => { return responseBody });

updateEnv({ 'LAST_UPDATE': assignmentResponse.data_updated_at })

const assignmentsTab = assignmentResponse.data;

while (assignmentResponse.pages.next_url) {
    const nextAssignmentApiEndpoint = new Request(assignmentResponse.pages.next_url);
    assignmentResponse = await fetch(nextAssignmentApiEndpoint)
        .then(response => response.json())
        .then(responseBody => { return responseBody });

    for (const assignment of assignmentResponse.data) {
        assignmentsTab.push(assignment);
    }
}

const itemIdTab = [];

for (const assignment of assignmentsTab) {
    itemIdTab.push(assignment.data.subject_id);
}
const subjectApiEndpoint =
    new Request('https://api.wanikani.com/v2/subjects' +
        '?ids=' + itemIdTab.join(), {
        method: 'GET',
        headers: requestHeaders
    });

const subjectResponse = await fetch(subjectApiEndpoint)
    .then(response => response.json())
    .then(responseBody => { return responseBody });

var subjectsTab = subjectResponse.data;

while (subjectResponse.pages.next_url) {
    const nextSubjectApiEndpoint = new Request(subjectResponse.pages.next_url);
    subjectResponse = await fetch(nextSubjectApiEndpoint)
        .then(response => response.json())
        .then(responseBody => { return responseBody });

    for (const subject of subjectResponse.data) {
        subjectsTab.push(subject);
    }
}

const items = { items: [] };

for (const subject of subjectsTab) {
    const meanings = [];
    const readings = [];
    for (const meaning of subject.data.meanings) {
        if (meaning.accepted_answer) {
            meanings.push(meaning.meaning.toLowerCase());
        }
    }
    for (const meaning of subject.data.auxiliary_meanings) {
        if (meaning.type === "whitelist") {
            meanings.push(meaning.meaning.toLowerCase());
        }
    }
    for (const reading of subject.data.readings) {
        if (reading.accepted_answer) {
            readings.push(reading.reading)
        }
    }
    const item = {
        id: subject.id,
        type: subject.object,
        characters: subject.data.characters,
        url: subject.data.document_url,
        meanings: meanings,
        readings: readings,
    };
    console.log("New burned item : " + item.characters);
    items.items.push(item);
}

fs.readFile('../data/burnedItems.json', 'utf8', function readFileCallback(err, data) {
    if (err) {
        console.log(err);
    } else {
        const obj = JSON.parse(data);
        for (const item of items.items) {
            obj.items.push(item);
        }
        const json = JSON.stringify(obj);
        fs.writeFile('../data/burnedItems.json', json, 'utf8', function writeFileCallback(err) {
            if (err) {
                console.log(err);
            }
        });
    }
});