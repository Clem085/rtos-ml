# To Run File from CMD:
#   powershell -NoProfile -ExecutionPolicy Bypass -File "C:\Users\consa\Downloads\Academics\NC_STATE\2025-2026\SPRING_2026\RTOS ML\RTOS-ARC\set-arc-node.ps1"

# To exit cluster node: 
#   ssh arc "scancel -u $USER"

# Auto-select ARC compute node and update ssh config
# Assumes clusters are c0–c99

# --- inputs ---
$SshDir  = "$env:USERPROFILE\.ssh"                 # ssh directory
$KeyPath = "C:\Users\consa\.ssh\arc_cluster"       # FULL PATH to private key file
$UnityId = "cbsavugo"                              # unity id / username
# -------------

$ssh = "$env:WINDIR\System32\OpenSSH\ssh.exe"
$cfg = "$SshDir\config"

function Step($msg) { Write-Host ""; Write-Host "==> $msg" }
function Info($msg) { Write-Host "    $msg" }
function Warn($msg) { Write-Host "    WARNING: $msg" }
function Fail($msg) { Write-Host "    ERROR: $msg" }

function Ensure-NewlineAtEOF([string]$text) {
    if ([string]::IsNullOrEmpty($text)) { return "" }
    if ($text -match "(\r?\n)$") { return $text }
    return $text + "`r`n"
}

function Normalize-HostSpacing([string]$text) {
    # Guarantee a blank line BEFORE every "Host " line (except if it's already blank)
    $lines = $text -split "`r?`n"
    $out   = New-Object System.Collections.Generic.List[string]

    foreach ($line in $lines) {
        if ($line -match '^\s*Host\s+') {
            if ($out.Count -gt 0 -and $out[$out.Count-1].Trim() -ne "") {
                $out.Add("")
            }
        }
        $out.Add($line)
    }

    return ($out -join "`r`n")
}

function Ensure-HostBlock([string]$configText, [string]$hostAlias, $kvOrdered) {
    # Replaces the entire Host <alias> block if present, otherwise appends it.
    # IMPORTANT: We intentionally add a trailing blank line after the block so
    # blocks can never glue together.
    $lines = $configText -split "`r?`n"

    $targetStart = -1
    $targetEnd = -1

    for ($i = 0; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match '^\s*Host\s+(.+)\s*$') {
            $hosts = $Matches[1]
            $isTarget = ($hosts -match ("(^|\s)" + [regex]::Escape($hostAlias) + "(\s|$)"))
            if ($isTarget) {
                $targetStart = $i
                $j = $i + 1
                while ($j -lt $lines.Count -and -not ($lines[$j] -match '^\s*Host\s+')) { $j++ }
                $targetEnd = $j - 1
                break
            }
        }
    }

    $desired = New-Object System.Collections.Generic.List[string]
    $desired.Add("Host $hostAlias")
    foreach ($kv in $kvOrdered) {
        $desired.Add(("  {0} {1}" -f $kv.Key, $kv.Value))
    }
    $desired.Add("")  # <- critical: blank line AFTER every generated Host block

    if ($targetStart -ge 0) {
        $out = New-Object System.Collections.Generic.List[string]
        for ($i = 0; $i -lt $targetStart; $i++) { $out.Add($lines[$i]) }
        foreach ($l in $desired) { $out.Add($l) }
        for ($i = $targetEnd + 1; $i -lt $lines.Count; $i++) { $out.Add($lines[$i]) }

        return @{
            Text   = ($out -join "`r`n")
            Action = "replaced"
            Block  = ($desired -join "`r`n")
        }
    }

    # append
    $t = $configText.TrimEnd()
    if ($t.Length -gt 0) { $t += "`r`n`r`n" }
    $t += ($desired -join "`r`n")

    return @{
        Text   = $t
        Action = "appended"
        Block  = ($desired -join "`r`n")
    }
}

function Write-Config([string]$path, [string]$text, [string]$reason) {
    $text = Ensure-NewlineAtEOF $text
    $text = Normalize-HostSpacing $text
    Set-Content -Path $path -Value $text -NoNewline
    Info "Wrote config file ($reason): $path"
}

Step "Starting set-arc-node"
Info "UnityId     = $UnityId"
Info "KeyPath     = $KeyPath"
Info "Config file = $cfg"
Info "ssh.exe     = $ssh"

if (-not (Test-Path $KeyPath)) { Fail "Key file not found: $KeyPath"; exit 1 }
if (-not (Test-Path $cfg)) { Fail "SSH config not found: $cfg"; Fail "Create the file (even empty) and re-run."; exit 1 }

# --------------------------------------------------------------------
# 1) FIX CONFIG FIRST (required): remove/fix blank HostName lines,
#    ensure arc + arc-node blocks are valid, and add newline spacing.
#    Only after this do we run any ssh.
# --------------------------------------------------------------------
Step "Fix + write SSH config BEFORE any ssh calls"

$configText = Get-Content $cfg -Raw

# 1a) Repair blank HostName/hostname lines so ssh can parse the file at all.
$lines = $configText -split "`r?`n"
$outLines = New-Object System.Collections.Generic.List[string]
$inArcNode = $false
$prefChanges = 0

for ($i = 0; $i -lt $lines.Count; $i++) {
    $line = $lines[$i]

    if ($line -match '^\s*Host\s+(.+)\s*$') {
        $hosts = $Matches[1]
        $inArcNode = ($hosts -match '(^|\s)arc-node(\s|$)')
    }

    # blank "HostName"/"hostname" line (case-insensitive), including "HostName   " or "hostname  #comment"
    if ($line -match '^(?i)\s*hostname(\s*(#.*)?)\s*$') {
        if ($inArcNode) {
            $indent = ($line -replace '^(\s*).*$', '$1')
            $outLines.Add($indent + "HostName c99")
            $prefChanges++
        } else {
            $prefChanges++
            continue
        }
    } else {
        $outLines.Add($line)
    }
}

$configText = ($outLines -join "`r`n")

# 1b) Enforce arc-node block FIRST (temporary HostName c99 until we find allocation)
$arcNodeRes = Ensure-HostBlock $configText "arc-node" @(
    @{ Key="HostName";        Value="c99" },
    @{ Key="User";            Value=$UnityId },
    @{ Key="IdentityFile";    Value=$KeyPath },
    @{ Key="IdentitiesOnly";  Value="yes" },
    @{ Key="ProxyJump";       Value="arc" }
)
$configText = $arcNodeRes.Text

# 1c) Enforce arc block
$arcRes = Ensure-HostBlock $configText "arc" @(
    @{ Key="HostName";     Value="arc.csc.ncsu.edu" },
    @{ Key="User";         Value=$UnityId },
    @{ Key="IdentityFile"; Value=$KeyPath }
)
$configText = $arcRes.Text

Write-Config $cfg $configText "pre-ssh (repair + ensure arc/arc-node). Blank HostName fixes=$prefChanges"

Info "arc-node block $($arcNodeRes.Action) (temporary HostName c99):"
($arcNodeRes.Block -split "`r?`n") | ForEach-Object { if ($_ -ne "") { Info $_ } }
Info "arc block $($arcRes.Action):"
($arcRes.Block -split "`r?`n") | ForEach-Object { if ($_ -ne "") { Info $_ } }

# --------------------------------------------------------------------
# 2) NOW it is safe to use ssh to query Slurm (config is valid)
# --------------------------------------------------------------------
Step "Check for existing Slurm allocation (ssh arc -> squeue)"
Info "Running: ssh -i `"$KeyPath`" arc 'squeue -u $UnityId -h -o %N | head -n 1'"

$node = (& $ssh -i $KeyPath arc "squeue -u $UnityId -h -o %N | head -n 1")
if ($node) { $node = $node.Trim() }

if ($LASTEXITCODE -ne 0) { Warn "ssh exit code $LASTEXITCODE. Output may be empty." }

if ([string]::IsNullOrWhiteSpace($node)) {
    Step "No allocation found. Request allocation (ssh arc -> salloc) and re-check"
    Info "Running: ssh -i `"$KeyPath`" arc 'salloc -p csc549 --no-shell sleep 5'"

    & $ssh -i $KeyPath arc "salloc -p csc549 --no-shell sleep 5" | Out-Null
    Start-Sleep 3

    Info "Running: ssh -i `"$KeyPath`" arc 'squeue -u $UnityId -h -o %N | head -n 1'"
    $node = (& $ssh -i $KeyPath arc "squeue -u $UnityId -h -o %N | head -n 1")
    if ($node) { $node = $node.Trim() }
}

if ([string]::IsNullOrWhiteSpace($node)) {
    Warn "Still no allocation detected. Keeping fallback node c99."
    $node = "c99"
}

Step "Node selection result"
Info "Selected node = $node"

# --------------------------------------------------------------------
# 3) UPDATE arc-node HostName to the selected node and write config again
# --------------------------------------------------------------------
Step "Write SSH config with final arc-node HostName = $node"

$configText = Get-Content $cfg -Raw

$arcNodeFinal = Ensure-HostBlock $configText "arc-node" @(
    @{ Key="HostName";        Value=$node },
    @{ Key="User";            Value=$UnityId },
    @{ Key="IdentityFile";    Value=$KeyPath },
    @{ Key="IdentitiesOnly";  Value="yes" },
    @{ Key="ProxyJump";       Value="arc" }
)

$configText = $arcNodeFinal.Text

# Re-enforce arc as well (keeps User/IdentityFile aligned with variables even if someone edits manually)
$arcFinal = Ensure-HostBlock $configText "arc" @(
    @{ Key="HostName";     Value="arc.csc.ncsu.edu" },
    @{ Key="User";         Value=$UnityId },
    @{ Key="IdentityFile"; Value=$KeyPath }
)
$configText = $arcFinal.Text

Write-Config $cfg $configText "final (set arc-node HostName=$node)"

Info "arc-node block $($arcNodeFinal.Action):"
($arcNodeFinal.Block -split "`r?`n") | ForEach-Object { if ($_ -ne "") { Info $_ } }
Info "arc block $($arcFinal.Action):"
($arcFinal.Block -split "`r?`n") | ForEach-Object { if ($_ -ne "") { Info $_ } }

Step "Done"
Info "Updated ssh config: arc-node -> $node"

