#![deny(warnings)]
#![warn(rust_2018_idioms)]
//use std::env;

use hyper::{body::HttpBody as _};
use tokio::io::{self, AsyncWriteExt as _};
use hyper::{client::conn::Builder, Body};
use hyper::http::{Request, StatusCode};

use tokio::net::windows::named_pipe;

const PIPE_NAME: &str = r"\\.\pipe\mynamedpipe";

// A simple type alias so as to DRY.
type Result<T> = std::result::Result<T, Box<dyn std::error::Error + Send + Sync>>;

#[tokio::main]
async fn main() -> Result<()> {
    let stream = named_pipe::ClientOptions::new().open(PIPE_NAME)?;

    //let mut res = client.get(url).await?;
    let (mut request_sender, connection) = Builder::new()
        .handshake::<named_pipe::NamedPipeClient, Body>(stream)
        .await?;
    
    tokio::spawn(async move {
        if let Err(e) = connection.await {
            eprintln!("Error in connection: {}", e);
        }
    });

    let request = Request::builder()
        .uri(String::from("/count"))
        .header("Host", "example.com")
        .method("GET")
        .body(Body::from("")).expect("cannot make request");

    let mut response = request_sender.send_request(request).await?;

    assert!(response.status() == StatusCode::OK);

    while let Some(next) = response.data().await {
        let chunk = next?;
        io::stdout().write_all(&chunk).await?;
    }

    Ok(())
}