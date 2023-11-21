use std::env;
use std::fs::{File, OpenOptions};
use std::io::{BufRead, BufReader, Write};
use std::collections::{HashSet, HashMap};

fn main() {
    let set_commands = ["SADD", "SREM", "SISMEMBER"];
    let stack_commands = ["SPUSH", "SPOP", "SPOP"];
    let queue_commands = ["QPUSH", "QPOP", "QPOP"];
    let hash_table_commands = ["HSET", "HDEL", "HGET"];

    let mut file = 0;
    let mut cmd = 0;

    for (i, arg) in env::args().enumerate() {
        match arg.as_str() {
            "--file" => file = i + 1,
            "--query" => cmd = i + 1,
            _ => {}
        }
    }

    if file >= env::args().len() || file == 0 {
        println!("Укажите флаг --file <имя файла>");
        return;
    }

    if cmd >= env::args().len() || cmd == 0 {
        println!("Укажите флаг --query <запрос>");
        return;
    }

    let filename = env::args().nth(file).unwrap();
    let query = env::args().skip(cmd).collect::<Vec<String>>();

    let mut work = "";
    let mut data: Box<dyn std::any::Any> = Box::new(());
    let size = 0;

    if set_commands.contains(&query[0].as_str()) {
        work = "set";
        data = Box::new(HashSet::<String>::new());
    } else if hash_table_commands.contains(&query[0].as_str()) {
        work = "hash";
        data = Box::new(HashMap::<String, String>::new());
    } else if stack_commands.contains(&query[0].as_str()) {
        work = "stack";
        data = Box::new(Vec::<String>::new());
    } else if queue_commands.contains(&query[0].as_str()) {
        work = "queue";
        data = Box::new(Vec::<String>::new());
    } else {
        println!("Ошибка запроса! Нет такой команды {}", query[0]);
        return;
    }

    let f = File::open(&filename).unwrap();
    let reader = BufReader::new(f);
    let mut head = String::new();
    let mut footer = String::new();
    let mut lines = reader.lines().map(|l| l.unwrap());

    for line in &mut lines {
        let split_line = line.trim().split(' ').collect::<Vec<&str>>();
        if split_line[0] == work && split_line[1] == query[1] {
            if work == "set" {
                let set_data = split_line[2..].iter().map(|s| s.to_string()).collect::<HashSet<String>>();
                data = Box::new(set_data);
            } else if work == "hash" {
                let mut hash_data = HashMap::new();
                for el in split_line[2..].iter() {
                    let kv = el.split(':').collect::<Vec<&str>>();
                    hash_data.insert(kv[0].to_string(), kv[1].to_string());
                }
                data = Box::new(hash_data);
            } else if work == "stack" {
                let stack_data = split_line[2..].iter().map(|s| s.to_string()).collect::<Vec<String>>();
                data = Box::new(stack_data);
            } else if work == "queue" {
                let queue_data = split_line[2..].iter().map(|s| s.to_string()).collect::<Vec<String>>();
                data = Box::new(queue_data);
            }
            footer = lines.collect::<Vec<String>>().join("\n");
            break;
        }
        head.push_str(line.as_str());
        head.push_str("\n");
    }

    match query[0].as_str() {
        "SADD" => {
            let set_data = data.downcast_mut::<HashSet<String>>().unwrap();
            set_data.insert(query[2].to_string());
        },
        "SREM" => {
            let set_data = data.downcast_mut::<HashSet<String>>().unwrap();
            if !set_data.remove(&query[2]) {
                println!("Такого элемента нет в множестве {}", query[1]);
                return;
            }
        },
        "SISMEMBER" => {
            let set_data = data.downcast_ref::<HashSet<String>>().unwrap();
            println!("{}", set_data.contains(&query[2]));
        },
        "HSET" => {
            let hash_data = data.downcast_mut::<HashMap<String, String>>().unwrap();
            hash_data.insert(query[2].to_string(), query[3].to_string());
        },
        "HDEL" => {
            let hash_data = data.downcast_mut::<HashMap<String, String>>().unwrap();
            if hash_data.remove(&query[2]).is_none() {
                println!("Такого ключа нет в таблице {}", query[1]);
                return;
            }
        },
        "HGET" => {
            let hash_data = data.downcast_ref::<HashMap<String, String>>().unwrap();
            if hash_data.contains_key(&query[2]) {
                println!("{}", hash_data.get(&query[2]).unwrap());
            }
        },
        "SPUSH" => {
            let stack_data = data.downcast_mut::<Vec<String>>().unwrap();
            stack_data.push(query[2].to_string());
        },
        "SPOP" => {
            let stack_data = data.downcast_mut::<Vec<String>>().unwrap();
            if let Some(val) = stack_data.pop() {
                println!("POPPED: {}", val);
            }
        },
        "QPUSH" => {
            let queue_data = data.downcast_mut::<Vec<String>>().unwrap();
            queue_data.push(query[2].to_string());
        },
        "QPOP" => {
            let queue_data = data.downcast_mut::<Vec<String>>().unwrap();
            println!("POPPED QUEUE: {}", queue_data[0]);
            queue_data.remove(0);
        },
        _ => {}
    }

    let mut new_data = String::new();
    match work {
        "set" => {
            let set_data = data.downcast_ref::<HashSet<String>>().unwrap();
            if !set_data.is_empty() {
                new_data = format!("{} {} {}\n", work, query[1], set_data.iter().map(|s| s.as_str()).collect::<Vec<&str>>().join(" "));
            }
        },
        "hash" => {
            let hash_data = data.downcast_ref::<HashMap<String, String>>().unwrap();
            if !hash_data.is_empty() {
                let include = hash_data.iter().map(|(k, v)| format!("{}:{}", k, v)).collect::<Vec<String>>().join(" ");
                new_data = format!("{} {} {}\n", work, query[1], include);
            }
        },
        "stack" => {
            let stack_data = data.downcast_ref::<Vec<String>>().unwrap();
            if !stack_data.is_empty() {
                new_data = format!("{} {} {}\n", work, query[1], stack_data.iter().map(|s| s.as_str()).collect::<Vec<&str>>().join(" "));
            }
        },
        "queue" => {
            let queue_data = data.downcast_ref::<Vec<String>>().unwrap();
            if !queue_data.is_empty() {
                new_data = format!("{} {} {}\n", work, query[1], queue_data.iter().map(|s| s.as_str()).collect::<Vec<&str>>().join(" "));
            }
        },
        _ => {}
    }

    let mut new_file_content = String::new();
    println!("HEAD:{}!!!\nNEW:{}!!!\nFOOTER:{}!!!\n", head, new_data, footer);
    new_file_content.push_str(head.as_str());
    new_file_content.push_str(new_data.as_str());
    new_file_content.push_str(footer.as_str());

    let mut f = OpenOptions::new().write(true).truncate(true).open(&filename).unwrap();
    f.write_all(new_file_content.as_bytes()).unwrap();
}
