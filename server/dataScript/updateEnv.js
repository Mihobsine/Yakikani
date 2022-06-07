import dotenv from 'dotenv'
import fs from 'fs'

const env = fs.readFileSync('.env')
const buf = Buffer.from(env)
const currentConfig = dotenv.parse(buf)

export function updateEnv(config = {}, eol = '\n') {
    const envContents = Object.entries({ ...currentConfig, ...config })
        .map(([key, val]) => `${key}=${val}`)
        .join(eol)
    fs.writeFileSync('.env', envContents);
}
