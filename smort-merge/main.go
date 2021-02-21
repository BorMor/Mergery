package main

import (
	"bytes"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path"
	"path/filepath"
)

//"smort-merge.exe" --base "$BASE" --local "$LOCAL" --remote "$REMOTE" --output "$MERGED" --engine "W:/EpicGames/UE_4.26/Engine/Binaries/Win64/UE4Editor.exe" --fallback "C:/Program Files/KDiff3/kdiff3.exe"
func main() {
	var err error
	var engine, fallback, base, local, remote, output string

	flag.StringVar(&base, "base", "", "")
	flag.StringVar(&local, "local", "", "")
	flag.StringVar(&remote, "remote", "", "")
	flag.StringVar(&output, "output", "", "")

	flag.StringVar(&engine, "engine", "", "")
	flag.StringVar(&fallback, "fallback", "", "")

	flag.Parse()

	if path.Ext(base) == ".uasset" {
		base, err = filepath.Abs(base)
		if err != nil {
			log.Fatal(err)
		}

		local, err = filepath.Abs(local)
		if err != nil {
			log.Fatal(err)
		}

		remote, err = filepath.Abs(remote)
		if err != nil {
			log.Fatal(err)
		}

		output, err = filepath.Abs(output)
		if err != nil {
			log.Fatal(err)
		}

		files, err := ioutil.ReadDir(".")
		if err != nil {
			log.Fatal(err)
		}

		var projectFile string
		for _, f := range files {
			if path.Ext(f.Name()) == ".uproject" {
				projectFile, err = filepath.Abs(f.Name())
				if err != nil {
					log.Fatal(err)
				}
			}
		}

		if projectFile == "" {
			log.Fatal(".uproject file not found")
		}

		fmt.Println("Project: ", projectFile)
		fmt.Print(base, "\n", local, "\n", remote, "\n", output, "\n")

		cmd := exec.Command(engine, projectFile, "-Mergery", "-MergeBase=", base, "-MergeLocal=", local, "-MergeRemote=", remote, "-MergeOutput=", output)
		err = cmd.Run()
		if err != nil {
			log.Fatal(err)
		}

		if cmd.ProcessState.ExitCode() != 0 {
			log.Fatal("Bad exit code")
		}

		file, err := os.Open(output)
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()

		byteSlice := make([]byte, 42)
		_, err = file.Read(byteSlice)
		if err != nil {
			log.Fatal(err)
		}

		if bytes.Contains(byteSlice, []byte("git-lfs.github.com")) {
			log.Fatal("Output is lfs pointer")
		}

	} else {
		var stdOut, stdErr bytes.Buffer
		cmd := exec.Command(fallback, base, local, remote, "-o", output)
		cmd.Stdout = &stdOut
		cmd.Stderr = &stdErr
		err = cmd.Run()

		fmt.Print("\n", stdOut.String(), "\n")
		fmt.Print("\n", stdErr.String(), "\n")

		if err != nil {
			log.Fatal(err)
		}

		if cmd.ProcessState.ExitCode() != 0 {
			log.Fatal("Bad exit code")
		}
	}
}
