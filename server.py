# encoding:utf-8
import web
import datetime
import db
import json

urls = (
    '/','Index',
    '/day/(.+)', 'Day',
    '/status', 'Status',
)
app = web.application(urls, globals())

DB = db.DB()

class Index:
    def GET(self):
        today = datetime.datetime.today()
        day = today.strftime("%Y-%m-%d")
        raise web.seeother('/day/%s' % day)

class Day:
    def GET(self, day):
        web.header('Content-Type', 'application/json')
        data = DB.getDay(day)
        result = []
        for one in data:
            result.append({"id":one[0], "temperature":one[1], "humidity":one[2], "time":one[3]})
        return json.dumps(result)

class Status:
    def GET(self):
        return '<h1>Server running...</h1>'

if __name__ == "__main__":
    app.run()
