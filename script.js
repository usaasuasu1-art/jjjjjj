const VERCEL_TOKEN = '69gi4EwELfMXLxy5nt2zEQ2j';

let selectedFiles = [];
let isDeploying = false;

// DOM Elements
const siteNameInput = document.getElementById('siteName');
const urlPreview = document.getElementById('urlPreview');
const fileInput = document.getElementById('fileInput');
const fileList = document.getElementById('fileList');
const fileListContent = document.getElementById('fileListContent');
const fileCount = document.getElementById('fileCount');
const deployButton = document.getElementById('deployButton');
const statusMessage = document.getElementById('statusMessage');
const deploymentLogs = document.getElementById('deploymentLogs');

// Update URL Preview
siteNameInput.addEventListener('input', (e) => {
    const name = e.target.value.toLowerCase().replace(/[^a-z0-9-]/g, '-');
    urlPreview.textContent = name ? `${name}.vercel.app` : 'nama-website.vercel.app';
});

// Handle File Selection
fileInput.addEventListener('change', (e) => {
    selectedFiles = Array.from(e.target.files);
    
    if (selectedFiles.length > 0) {
        fileList.style.display = 'block';
        fileCount.textContent = selectedFiles.length;
        
        fileListContent.innerHTML = '';
        selectedFiles.slice(0, 10).forEach(file => {
            const fileItem = document.createElement('div');
            fileItem.className = 'file-item';
            const path = file.webkitRelativePath || file.name;
            const size = (file.size / 1024).toFixed(2);
            fileItem.textContent = `• ${path} (${size} KB)`;
            fileListContent.appendChild(fileItem);
        });
        
        if (selectedFiles.length > 10) {
            const moreItem = document.createElement('div');
            moreItem.className = 'file-item';
            moreItem.style.fontStyle = 'italic';
            moreItem.textContent = `...dan ${selectedFiles.length - 10} file lainnya`;
            fileListContent.appendChild(moreItem);
        }
        
        addLog(`${selectedFiles.length} file dipilih`);
    }
});

// Add Log Function
function addLog(message) {
    deploymentLogs.style.display = 'block';
    const time = new Date().toLocaleTimeString();
    const logEntry = document.createElement('div');
    logEntry.className = 'log-entry';
    logEntry.textContent = `[${time}] ${message}`;
    deploymentLogs.appendChild(logEntry);
    deploymentLogs.scrollTop = deploymentLogs.scrollHeight;
}

// Show Status
function showStatus(type, message, url = null) {
    statusMessage.style.display = 'flex';
    statusMessage.className = `status-message status-${type}`;
    
    const icons = {
        success: `<svg class="status-icon" viewBox="0 0 24 24" fill="none" stroke="#10b981" stroke-width="2"><path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"></path><polyline points="22 4 12 14.01 9 11.01"></polyline></svg>`,
        error: `<svg class="status-icon" viewBox="0 0 24 24" fill="none" stroke="#dc2626" stroke-width="2"><circle cx="12" cy="12" r="10"></circle><line x1="15" y1="9" x2="9" y2="15"></line><line x1="9" y1="9" x2="15" y2="15"></line></svg>`,
        info: `<svg class="status-icon spinner" viewBox="0 0 24 24" fill="none" stroke="#2563eb" stroke-width="2"><line x1="12" y1="2" x2="12" y2="6"></line><line x1="12" y1="18" x2="12" y2="22"></line><line x1="4.93" y1="4.93" x2="7.76" y2="7.76"></line><line x1="16.24" y1="16.24" x2="19.07" y2="19.07"></line><line x1="2" y1="12" x2="6" y2="12"></line><line x1="18" y1="12" x2="22" y2="12"></line><line x1="4.93" y1="19.07" x2="7.76" y2="16.24"></line><line x1="16.24" y1="7.76" x2="19.07" y2="4.93"></line></svg>`
    };
    
    statusMessage.innerHTML = `
        ${icons[type]}
        <div class="status-content">
            <p class="status-text">${message}</p>
            ${url ? `<a href="${url}" target="_blank" rel="noopener noreferrer" class="status-link">🚀 Buka Website: ${url}</a>` : ''}
        </div>
    `;
}

// Read File as Base64
function readFileAsBase64(file) {
    return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = () => {
            const base64 = reader.result.split(',')[1];
            resolve(base64);
        };
        reader.onerror = reject;
        reader.readAsDataURL(file);
    });
}

// Deploy Function
async function handleDeploy() {
    const siteName = siteNameInput.value.trim();
    
    if (!siteName) {
        showStatus('error', 'Nama website harus diisi!');
        return;
    }
    
    if (selectedFiles.length === 0) {
        showStatus('error', 'Pilih minimal satu file untuk di-upload!');
        return;
    }
    
    isDeploying = true;
    deployButton.disabled = true;
    deployButton.innerHTML = `
        <svg class="icon-deploy spinner" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
            <line x1="12" y1="2" x2="12" y2="6"></line>
            <line x1="12" y1="18" x2="12" y2="22"></line>
            <line x1="4.93" y1="4.93" x2="7.76" y2="7.76"></line>
            <line x1="16.24" y1="16.24" x2="19.07" y2="19.07"></line>
            <line x1="2" y1="12" x2="6" y2="12"></line>
            <line x1="18" y1="12" x2="22" y2="12"></line>
            <line x1="4.93" y1="19.07" x2="7.76" y2="16.24"></line>
            <line x1="16.24" y1="7.76" x2="19.07" y2="4.93"></line>
        </svg>
        <span>Deploying...</span>
    `;
    
    deploymentLogs.innerHTML = '';
    showStatus('info', 'Memulai deployment...');
    addLog('Memulai proses deployment ke Vercel');
    
    try {
        // Prepare files
        addLog(`Membaca ${selectedFiles.length} file...`);
        const fileObjects = {};
        
        // Find the common root path to remove it
        let commonPath = '';
        if (selectedFiles.length > 0 && selectedFiles[0].webkitRelativePath) {
            const firstPath = selectedFiles[0].webkitRelativePath;
            const pathParts = firstPath.split('/');
            if (pathParts.length > 1) {
                commonPath = pathParts[0] + '/';
            }
        }
        
        for (const file of selectedFiles) {
            const content = await readFileAsBase64(file);
            let filePath = file.webkitRelativePath || file.name;
            
            // Remove common root path so index.html is at root
            if (commonPath && filePath.startsWith(commonPath)) {
                filePath = filePath.substring(commonPath.length);
            }
            
            fileObjects[filePath] = {
                file: content,
                encoding: 'base64'
            };
            addLog(`✓ ${filePath} siap`);
        }
        
        // Check for index.html
        const hasIndex = Object.keys(fileObjects).some(path => 
            path === 'index.html'
        );
        
        if (!hasIndex) {
            showStatus('error', 'Error: Tidak ada file index.html di root folder. Pastikan index.html ada di folder utama yang Anda upload');
            return;
        }
        
        addLog(`File index.html ditemukan di root`);
        addLog('Mengirim ke Vercel API...');
        
        // Deploy to Vercel
        const deploymentResponse = await fetch('https://api.vercel.com/v13/deployments', {
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${VERCEL_TOKEN}`,
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                name: siteName.toLowerCase().replace(/[^a-z0-9-]/g, '-'),
                files: Object.keys(fileObjects).map(path => ({
                    file: path,
                    data: fileObjects[path].file,
                    encoding: 'base64'
                })),
                projectSettings: {
                    framework: null
                },
                target: 'production'
            })
        });
        
        if (!deploymentResponse.ok) {
            const errorData = await deploymentResponse.json();
            throw new Error(errorData.error?.message || 'Deployment gagal');
        }
        
        const deploymentData = await deploymentResponse.json();
        addLog('✓ Deployment dibuat');
        addLog(`Deployment ID: ${deploymentData.id}`);
        
        const url = `https://${deploymentData.url}`;
        showStatus('success', 'Website berhasil di-deploy ke Vercel!', url);
        addLog('✓ Deployment berhasil!');
        
        // Check status
        addLog('Memeriksa status deployment...');
        checkDeploymentStatus(deploymentData.id);
        
    } catch (error) {
        console.error('Deployment error:', error);
        addLog(`✗ Error: ${error.message}`);
        showStatus('error', `Deployment gagal: ${error.message}`);
    } finally {
        isDeploying = false;
        deployButton.disabled = false;
        deployButton.innerHTML = `
            <svg class="icon-deploy" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                <circle cx="12" cy="12" r="10"></circle>
                <line x1="2" y1="12" x2="22" y2="12"></line>
                <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"></path>
            </svg>
            <span>Deploy ke Vercel</span>
        `;
    }
}

// Check Deployment Status
async function checkDeploymentStatus(deploymentId, attempts = 0) {
    const maxAttempts = 30;
    
    try {
        const response = await fetch(`https://api.vercel.com/v13/deployments/${deploymentId}`, {
            headers: {
                'Authorization': `Bearer ${VERCEL_TOKEN}`
            }
        });
        
        if (response.ok) {
            const data = await response.json();
            addLog(`Status: ${data.readyState}`);
            
            if (data.readyState === 'READY') {
                addLog('✓ Website sudah online dan siap diakses!');
                return;
            } else if (data.readyState === 'ERROR') {
                addLog('⚠ Deployment error, tapi URL mungkin masih bisa diakses');
                return;
            }
        }
        
        if (attempts < maxAttempts) {
            setTimeout(() => checkDeploymentStatus(deploymentId, attempts + 1), 2000);
        }
    } catch (error) {
        console.error('Error checking status:', error);
    }
}

// Deploy Button Click
deployButton.addEventListener('click', handleDeploy);
